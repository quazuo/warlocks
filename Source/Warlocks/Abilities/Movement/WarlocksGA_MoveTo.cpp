#include "WarlocksGA_MoveTo.h"

#include "AbilitySystemComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "WarlocksAT_MoveTo.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

UWarlocksGA_MoveTo::UWarlocksGA_MoveTo()
{
	const FGameplayTag MoveTag = FGameplayTag::RequestGameplayTag("Ability.MoveTo");
	AbilityTags.AddTag(MoveTag);

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	ActivationBlockedTags.AddTag(StunTag);

	const FGameplayTag SpellTag = FGameplayTag::RequestGameplayTag("Ability.Spell");
	CancelAbilitiesWithTag.AddTag(SpellTag);
}

void UWarlocksGA_MoveTo::ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                         FGameplayTag ApplicationTag)
{
	Super::ActivateAbilityWithTargetData(TargetDataHandle, ApplicationTag);
	
	const auto Controller = Cast<AWarlocksPlayerController>(CurrentActorInfo->PlayerController);
	if (Controller)
	{
		UWarlocksAT_MoveTo* Task = UWarlocksAT_MoveTo::MoveTo(this, Controller, CachedTarget);
		Task->OnFinish.AddDynamic(this, &UWarlocksGA_MoveTo::OnCompleted);
		Task->ReadyForActivation();
	}
}

void UWarlocksGA_MoveTo::ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo,
                                                      const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	Super::ActivateLocalPlayerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), FXCursor, CachedTarget);
}

void UWarlocksGA_MoveTo::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ActorInfo->PlayerController.IsValid())
	{
		ActorInfo->PlayerController->StopMovement();
	}
}

void UWarlocksGA_MoveTo::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
