#include "WarlocksGA_MoveTo.h"

#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Tasks/WarlocksAT_MoveTo.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

UWarlocksGA_MoveTo::UWarlocksGA_MoveTo()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	ActivationBlockedTags.AddTag(StunTag);
}

void UWarlocksGA_MoveTo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo,
                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const auto Controller = Cast<AWarlocksPlayerController>(ActorInfo ? ActorInfo->PlayerController : nullptr);
	if (!Controller)
		return;

	FHitResult Hit;
	if (!Controller->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		return;

	if (ActiveTask)
	{
		ActiveTask->EndTask();
	}
	ActiveTask = UWarlocksAT_MoveTo::MoveTo(this, Controller, Hit.Location);
	ActiveTask->OnFinish.AddDynamic(this, &UWarlocksGA_MoveTo::OnCompleted);
	ActiveTask->ReadyForActivation();
}

void UWarlocksGA_MoveTo::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	ActiveTask->EndTask();
}

void UWarlocksGA_MoveTo::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
