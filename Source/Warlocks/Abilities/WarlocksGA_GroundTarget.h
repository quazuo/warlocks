#pragma once
#include "WarlocksGA_ClientToServer.h"

#include "WarlocksGA_GroundTarget.generated.h"

UCLASS(Abstract)
class UWarlocksGA_GroundTarget : public UWarlocksGA_ClientToServer
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                           FGameplayTag ApplicationTag) override;

	virtual void ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
	                                        const FGameplayAbilityActorInfo* ActorInfo,
	                                        const FGameplayAbilityActivationInfo ActivationInfo,
	                                        const FGameplayEventData* TriggerEventData) override;

	FVector CachedTarget;
};
