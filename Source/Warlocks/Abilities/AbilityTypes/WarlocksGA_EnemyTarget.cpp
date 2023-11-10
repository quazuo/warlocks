#include "WarlocksGA_EnemyTarget.h"

#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

void UWarlocksGA_EnemyTarget::ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                            FGameplayTag ApplicationTag)
{
	Super::ActivateAbilityWithTargetData(TargetDataHandle, ApplicationTag);

	// retrieve data
	const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0);
	if (!TargetData)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// decode data
	//const TArray<TWeakObjectPtr<AActor>> TargetActors = TargetData->GetActors();
	const auto TargetActor = TargetData->GetHitResult()->GetActor();
	if (!TargetActor)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}
	
	CachedTarget = Cast<AWarlocksCharacter>(TargetActor);
	if (!CachedTarget)
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UWarlocksGA_EnemyTarget::ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
                                                         const FGameplayAbilityActorInfo* ActorInfo,
                                                         const FGameplayAbilityActivationInfo ActivationInfo,
                                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateLocalPlayerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	const auto Controller = Cast<AWarlocksPlayerController>(ActorInfo->PlayerController);
	check(Controller);
	
	// Compute the TargetData
	FHitResult Hit;
	if (!Controller->GetHitResultUnderCursor(ECC_Pawn, true, Hit))
		return;

	const auto TargetWarlock = Cast<AWarlocksCharacter>(Hit.GetActor());
	if (!TargetWarlock || TargetWarlock == GetAvatarActorFromActorInfo())
		return;
	
	// Format the TargetData for GAS RPC

	// USE OF new() IS **REQUIRED**
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData->HitResult = Hit;

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);

	// Notify self (local client) *AND* server that TargetData is ready to be processed
	NotifyTargetDataReady(TargetDataHandle, FGameplayTag()); // send with a gameplay tag, or empty
}
