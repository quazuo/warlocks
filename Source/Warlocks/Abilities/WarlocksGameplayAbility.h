#pragma once

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

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo,
	                           bool bReplicateCancelAbility) override;
	// ~ UGameplayAbility overrides

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spell)
	FName SpellName;

	// time it takes to "wind up" before the spell. set to 0 if it should happen immediately
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Spell)
	float CastTime = 0.f;

	// gameplay effect applied to the character while casting the spell
	UPROPERTY(EditDefaultsOnly, Category = Spell)
	TSubclassOf<UGameplayEffect> CastTimeGE;

	// this most certainly doesn't need to be overridden, but the option is there
	virtual void StartSpellCast(const FGameplayAbilitySpecHandle Handle,
	                            const FGameplayAbilityActorInfo* ActorInfo,
	                            const FGameplayAbilityActivationInfo ActivationInfo,
	                            const FGameplayEventData* TriggerEventData);

	// this should be overridden and implemented
	UFUNCTION()
	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData);

	// time it takes to channel the spell
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Channel")
	float ChannelTime = 0.f;

	// gameplay effect applied to the character while channelling the spell
	UPROPERTY(EditDefaultsOnly, Category = "Spell|Channel")
	TSubclassOf<UGameplayEffect> ChannelGE;

	// called every tick while channelling the spell.
	// if relevant, this should be overridden and implemented
	UFUNCTION()
	virtual void OnChannelTick(const float DeltaTime)
	{
	}

	// called at the end of channelling the spell.
	UFUNCTION()
	virtual void OnChannelFinish(FGameplayTag EventTag, FGameplayEventData EventData);

	// cooldown related stuff

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Cooldown")
	float CooldownDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spell|Cooldown")
	FGameplayTagContainer CooldownTags;

protected:
	// Temp container that we will return the pointer to in GetCooldownTags().
	// This will be a union of our CooldownTags and the Cooldown GE's cooldown tags.
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;

	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;

	FGameplayTag SpellCastTag, ChannelTag;
};
