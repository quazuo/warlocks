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

	// this should be overridden and implemented
	UFUNCTION()
	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData);

	// this should be overridden and implemented
	virtual void ActivateServerAbility(const FGameplayAbilitySpecHandle Handle,
	                                   const FGameplayAbilityActorInfo* ActorInfo,
	                                   const FGameplayAbilityActivationInfo ActivationInfo,
	                                   const FGameplayEventData* TriggerEventData)
	{
	}

	// this should be overridden and implemented
	virtual void ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
	                                        const FGameplayAbilityActorInfo* ActorInfo,
	                                        const FGameplayAbilityActivationInfo ActivationInfo,
	                                        const FGameplayEventData* TriggerEventData)
	{
	}

	bool bActivateAbilityOnGranted = false;

	// if this ability is a spell, this is its name, which should match the name of some row in the spells' data table
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SpellName;

	// time it takes to "wind up" before the spell
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CastTime;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CastTimeGE;

	// cooldown related stuff
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cooldown")
	float CooldownDuration;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldown")
	FGameplayTagContainer CooldownTags;

	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;

private:
	FGameplayTag SpellCastTag, ChannelTag;
};
