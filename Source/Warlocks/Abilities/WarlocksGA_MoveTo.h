#pragma once

#include "CoreMinimal.h"
#include "WarlocksGA_GroundTarget.h"
#include "Abilities/GameplayAbility.h"
#include "WarlocksGA_MoveTo.generated.h"

class UWarlocksAT_MoveTo;
UCLASS()
class WARLOCKS_API UWarlocksGA_MoveTo final : public UWarlocksGA_GroundTarget
{
	GENERATED_BODY()

public:
	UWarlocksGA_MoveTo();

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo,
	                           bool bReplicateCancelAbility) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                           FGameplayTag ApplicationTag) override;

	virtual void ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
	                                        const FGameplayAbilityActorInfo* ActorInfo,
	                                        const FGameplayAbilityActivationInfo ActivationInfo,
	                                        const FGameplayEventData* TriggerEventData) override;

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	UPROPERTY(EditDefaultsOnly, Category = Input)
	class UNiagaraSystem* FXCursor;

	UPROPERTY()
	UWarlocksAT_MoveTo* ActiveTask = nullptr;
};
