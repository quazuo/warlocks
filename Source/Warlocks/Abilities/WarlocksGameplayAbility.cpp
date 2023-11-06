#include "WarlocksGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "WarlocksAbilitySystemComponent.h"
#include "Tasks/WarlocksAT_Delay.h"
#include "Tasks/WarlocksAT_DelayTick.h"
#include "Warlocks/Warlocks.h"

UWarlocksGameplayAbility::UWarlocksGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpellCastTag = FGameplayTag::RequestGameplayTag("Player.State.SpellCast");
	ChannelTag = FGameplayTag::RequestGameplayTag("Player.State.Channel");

	const FGameplayTag MoveTag = FGameplayTag::RequestGameplayTag("Ability.MoveTo");
	const FGameplayTag SpellTag = FGameplayTag::RequestGameplayTag("Ability.Spell");
	CancelAbilitiesWithTag.AddTag(MoveTag);
	CancelAbilitiesWithTag.AddTag(SpellTag);

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	const FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag("Player.State.Dead");
	ActivationBlockedTags.AddTag(StunTag);
	ActivationBlockedTags.AddTag(DeadTag);
}

void UWarlocksGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UWarlocksGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
		// if i'm a server for a remote player
		ActivateServerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	else
		// if i'm a locally controlled player (perhaps with authority)
		ActivateLocalPlayerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UWarlocksGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	RemoveGameplayEffectFromOwner({SpellCastTag, ChannelTag});
}

void UWarlocksGameplayAbility::StartSpellCast(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	if (CastTime > 0.f && CastTimeGE)
	{
		ApplyGameplayEffectToOwner(CastTimeGE);

		UWarlocksAT_Delay* Task = UWarlocksAT_Delay::TaskDelay(this, CastTime);
		Task->OnFinish.AddDynamic(this, &ThisClass::OnSpellCastFinish);
		Task->ReadyForActivation();

		if (const auto ASC = Cast<UWarlocksAbilitySystemComponent>(ActorInfo->AbilitySystemComponent))
		{
			ASC->SpellCastTask = Task;
		}
	}
	else
	{
		OnSpellCastFinish({}, {});
	}
}

void UWarlocksGameplayAbility::OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	RemoveGameplayEffectFromOwner(SpellCastTag);

	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}

	if (ChannelTime > 0.f && ChannelGE)
	{
		StartChannel();
	}
}

void UWarlocksGameplayAbility::StartChannel()
{
	ApplyGameplayEffectToOwner(ChannelGE);

	UWarlocksAT_DelayTick* Task = UWarlocksAT_DelayTick::TaskDelayTick(this, ChannelTime);
	Task->OnFinish.AddDynamic(this, &ThisClass::OnChannelFinish);
	Task->OnTick.AddDynamic(this, &ThisClass::OnChannelTick);
	Task->ReadyForActivation();

	if (const auto ASC = Cast<UWarlocksAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent))
	{
		ASC->ChannelTask = Task;
	}
}

void UWarlocksGameplayAbility::OnChannelFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	RemoveGameplayEffectFromOwner(ChannelTag);
}

const FGameplayTagContainer* UWarlocksGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);

	// MutableTags writes to the TempCooldownTags on the CDO so clear it in case the ability cooldown tags change
	// (moved to a different slot)
	MutableTags->Reset();
	if (const FGameplayTagContainer* ParentTags = Super::GetCooldownTags())
	{
		MutableTags->AppendTags(*ParentTags);
	}
	MutableTags->AppendTags(CooldownTags);

	return MutableTags;
}

void UWarlocksGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo,
                                             const FGameplayAbilityActivationInfo ActivationInfo) const
{
	const UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (!CooldownGE) return;

	const FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());

	SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Cooldown"),
	                                               CooldownDuration);

	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}

FActiveGameplayEffectHandle
UWarlocksGameplayAbility::ApplyGameplayEffectToOwner(TSubclassOf<UGameplayEffect> Class) const
{
	if (!Class)
	{
		UE_LOG(LogWarlocks, Warning, TEXT("ApplyGameplayEffectToOwner() called with an invalid Class"));
		return {};
	}
	
	const UGameplayEffect* EffectCDO = Class->GetDefaultObject<UGameplayEffect>();
	return Super::ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectCDO, 1);
}

FActiveGameplayEffectHandle
UWarlocksGameplayAbility::ApplySetByCallerEffectToOwner(const TSubclassOf<UGameplayEffect> Class,
                                                        const FGameplayTag DataTag, const float Magnitude) const
{
	if (!Class)
	{
		UE_LOG(LogWarlocks, Warning, TEXT("ApplySetByCallerEffectToOwner() called with an invalid Class"));
		return {};
	}
	
	const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(Class, GetAbilityLevel());
	EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(DataTag, Magnitude);
	return ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle);
}

void UWarlocksGameplayAbility::RemoveGameplayEffectFromOwner(const FGameplayTag Tag) const
{
	const auto ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	const FGameplayTagContainer GrantedTags{Tag};
	ASC->RemoveActiveEffectsWithGrantedTags(GrantedTags);
}

void UWarlocksGameplayAbility::RemoveGameplayEffectFromOwner(const std::initializer_list<FGameplayTag> Tags) const
{
	const auto ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC) return;

	FGameplayTagContainer GrantedTags;

	for (const auto& Tag : Tags)
	{
		GrantedTags.AddTag(Tag);
	}

	ASC->RemoveActiveEffectsWithGrantedTags(GrantedTags);
}
