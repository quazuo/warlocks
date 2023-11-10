#include "WarlocksGA_GroundTarget.h"

#include "Warlocks/Player/WarlocksPlayerController.h"

void UWarlocksGA_GroundTarget::ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                             FGameplayTag ApplicationTag)
{
	Super::ActivateAbilityWithTargetData(TargetDataHandle, ApplicationTag);

	// retrieve data
	const FGameplayAbilityTargetData* TargetData = TargetDataHandle.Get(0);
	if (!TargetData)
	{
		// client sent us bad data
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
		return;
	}

	// decode data
	CachedTarget = TargetData->GetEndPoint();
}

void UWarlocksGA_GroundTarget::ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
                                                          const FGameplayAbilityActorInfo* ActorInfo,
                                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateLocalPlayerAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const auto Controller = Cast<AWarlocksPlayerController>(ActorInfo->PlayerController);
	check(Controller);

	// Compute the TargetData
	FHitResult Hit;
	if (!Controller->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		return;

	const auto CursorLocation = Hit.Location;

	// Format the TargetData for GAS RPC

	// USE OF new() IS **REQUIRED**
	FGameplayAbilityTargetData_LocationInfo* TargetData = new FGameplayAbilityTargetData_LocationInfo();
	TargetData->TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	TargetData->TargetLocation.LiteralTransform = FTransform(CursorLocation);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(TargetData);

	// Notify self (local client) *AND* server that TargetData is ready to be processed
	NotifyTargetDataReady(TargetDataHandle, FGameplayTag()); // send with a gameplay tag, or empty
}
