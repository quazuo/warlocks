#pragma once

#include "Warlocks/Abilities/WarlocksGameplayAbility.h"
#include "WarlocksGA_Meditate.generated.h"

UCLASS()
class UWarlocksGA_Meditate final : public UWarlocksGameplayAbility
{
	GENERATED_BODY()

public:
	UWarlocksGA_Meditate();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWarlocksMeditateAura> MeditateAuraClass;

	UPROPERTY()
	AWarlocksMeditateAura* MeditateAura;
};
