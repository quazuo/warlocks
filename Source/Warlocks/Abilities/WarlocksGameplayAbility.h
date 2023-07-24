#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarlocksGameplayAbility.generated.h"

UCLASS(Abstract)
class WARLOCKS_API UWarlocksGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UWarlocksGameplayAbility();

	// UGameplayAbility overrides
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;
	// ~ UGameplayAbility overrides

	// this most certainly doesn't need to be overridden, but the option is there
	virtual void StartSpellCast(const FGameplayAbilitySpecHandle Handle,
	                            const FGameplayAbilityActorInfo* ActorInfo,
	                            const FGameplayAbilityActivationInfo ActivationInfo,
	                            const FGameplayEventData* TriggerEventData);

	// this NEEDS to be overridden and implemented
	UFUNCTION()
	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData);

	// this NEEDS to be overridden and implemented
	virtual void ActivateServerAbility(const FGameplayAbilitySpecHandle Handle,
	                                   const FGameplayAbilityActorInfo* ActorInfo,
	                                   const FGameplayAbilityActivationInfo ActivationInfo,
	                                   const FGameplayEventData* TriggerEventData);

	// this NEEDS to be overridden and implemented
	virtual void ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
	                                        const FGameplayAbilityActorInfo* ActorInfo,
	                                        const FGameplayAbilityActivationInfo ActivationInfo,
	                                        const FGameplayEventData* TriggerEventData);

	bool bActivateAbilityOnGranted = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SpellName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CastTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GE_OnActivate;

private:
	FGameplayTag SpellCastTag, ChannelTag;
};
