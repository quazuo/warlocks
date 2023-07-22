#pragma once

#include "CoreMinimal.h"
#include "WarlocksGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "WarlocksGA_MoveTo.generated.h"

class UWarlocksAT_MoveTo;
UCLASS()
class WARLOCKS_API UWarlocksGA_MoveTo final : public UWarlocksGameplayAbility
{
	GENERATED_BODY()

public:
	UWarlocksGA_MoveTo();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							   const FGameplayAbilityActivationInfo ActivationInfo,
							   bool bReplicateCancelAbility) override;

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	UPROPERTY()
	UWarlocksAT_MoveTo* ActiveTask = nullptr;
};
