#include "WarlocksGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "WarlocksAbilitySystemComponent.h"
#include "Tasks/WarlocksAT_Delay.h"
#include "Tasks/WarlocksAT_DelayTick.h"

UWarlocksGameplayAbility::UWarlocksGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	SpellCastTag = FGameplayTag::RequestGameplayTag("Player.State.SpellCast");
	ChannelTag = FGameplayTag::RequestGameplayTag("Player.State.Channel");

	const FGameplayTag MoveTag = FGameplayTag::RequestGameplayTag("Ability.MoveTo");
	CancelAbilitiesWithTag.AddTag(MoveTag);
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

void UWarlocksGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
	if (const auto ASC = GetAbilitySystemComponentFromActorInfo())
	{
		FGameplayTagContainer GrantedTags{SpellCastTag};
		GrantedTags.AddTag(ChannelTag);
		ASC->RemoveActiveEffectsWithGrantedTags(GrantedTags);
	}
}

void UWarlocksGameplayAbility::StartSpellCast(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	if (CastTime > 0.f && CastTimeGE)
	{
		const UGameplayEffect* EffectCDO = CastTimeGE->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle Effect =
			ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, EffectCDO, 1);

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
	if (const auto ASC = GetAbilitySystemComponentFromActorInfo())
	{
		const FGameplayTagContainer GrantedTags{SpellCastTag};
		ASC->RemoveActiveEffectsWithGrantedTags(GrantedTags);
	}

	if (!CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}

	if (ChannelTime > 0.f && ChannelGE)
	{
		const UGameplayEffect* EffectCDO = ChannelGE->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle Effect =
			ApplyGameplayEffectToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectCDO, 1);

		UWarlocksAT_DelayTick* Task = UWarlocksAT_DelayTick::TaskDelayTick(this, ChannelTime);
		Task->OnFinish.AddDynamic(this, &ThisClass::OnChannelFinish);
		Task->OnTick.AddDynamic(this, &ThisClass::OnChannelTick);
		Task->ReadyForActivation();

		if (const auto ASC = Cast<UWarlocksAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent))
		{
			ASC->ChannelTask = Task;
		}
	}
}

void UWarlocksGameplayAbility::OnChannelFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	
	if (const auto ASC = GetAbilitySystemComponentFromActorInfo())
	{
		const FGameplayTagContainer GrantedTags{ChannelTag};
		ASC->RemoveActiveEffectsWithGrantedTags(GrantedTags);
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
	if (!CooldownGE)
		return;
	
	const FGameplayEffectSpecHandle SpecHandle =
		MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
	
	SpecHandle.Data.Get()->DynamicGrantedTags.AppendTags(CooldownTags);
	SpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Data.Cooldown"),
	                                               CooldownDuration);
	
	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
}
