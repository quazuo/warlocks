#pragma once

#include "CoreMinimal.h"
#include "Warlocks/Abilities/WarlocksGameplayAbility.h"
#include "WarlocksGA_Fireball.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksGA_Fireball : public UWarlocksGameplayAbility
{
	GENERATED_BODY()

public:
	UWarlocksGA_Fireball();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                                const FGameplayTagContainer* SourceTags = nullptr,
	                                const FGameplayTagContainer* TargetTags = nullptr,
	                                OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

private:
	UPROPERTY()
	TSubclassOf<class AWarlocksFireball> FireballClass;

	FVector CachedTarget;
	
	UPROPERTY(EditDefaultsOnly)
	float CastTime = 1.f;
};
