#pragma once

#include "Warlocks/Abilities/WarlocksGameplayAbility.h"
#include "WarlocksGA_ClientToServer.generated.h"

UCLASS(Abstract)
class WARLOCKS_API UWarlocksGA_ClientToServer : public UWarlocksGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                           FGameplayTag ApplicationTag);

	UFUNCTION()
	virtual void NotifyTargetDataReady(const FGameplayAbilityTargetDataHandle& InData,
	                                   FGameplayTag ApplicationTag);

	virtual void ActivateServerAbility(const FGameplayAbilitySpecHandle Handle,
	                                   const FGameplayAbilityActorInfo* ActorInfo,
	                                   const FGameplayAbilityActivationInfo ActivationInfo,
	                                   const FGameplayEventData* TriggerEventData) override;

private:
	FDelegateHandle NotifyTargetDataReadyDelegateHandle;
};
