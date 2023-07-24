#include "WarlocksGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "WarlocksAbilitySystemComponent.h"
#include "Tasks/WarlocksAT_CastSpell.h"

UWarlocksGameplayAbility::UWarlocksGameplayAbility()
{
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
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CurrentActorInfo->IsNetAuthority() && !CurrentActorInfo->IsLocallyControlled())
		// if i'm a server for a remote player
		ActivateServerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	else
		// if i'm a locally controlled player (perhaps with authority)
		ActivateLocalPlayerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UWarlocksGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	const FGameplayTagContainer GrantedTags{SpellCastTag};
	ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(GrantedTags);
	BP_RemoveGameplayEffectFromOwnerWithGrantedTags(GrantedTags);
}

void UWarlocksGameplayAbility::StartSpellCast(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	if (CastTime > 0.f)
	{
		const UGameplayEffect* EffectCDO = GE_OnActivate->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle Effect =
			ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, EffectCDO, 1);

		UWarlocksAT_CastSpell* Task = UWarlocksAT_CastSpell::TaskCastSpell(this, CastTime);
		Task->OnFinish.AddDynamic(this, &ThisClass::OnSpellCastFinish);
		Task->ReadyForActivation();

		const auto ASC = Cast<UWarlocksAbilitySystemComponent>(ActorInfo->AbilitySystemComponent);
		ASC->SpellCastTask = Task;
	}
	else
	{
		OnSpellCastFinish({}, {});
	}
}

void UWarlocksGameplayAbility::ActivateServerAbility(const FGameplayAbilitySpecHandle Handle,
                                                     const FGameplayAbilityActorInfo* ActorInfo,
                                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                                     const FGameplayEventData* TriggerEventData)
{
}

void UWarlocksGameplayAbility::ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo,
                                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                                          const FGameplayEventData* TriggerEventData)
{
}

void UWarlocksGameplayAbility::OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
}
