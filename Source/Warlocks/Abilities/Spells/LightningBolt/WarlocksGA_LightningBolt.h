#pragma once

#include "CoreMinimal.h"
#include "Warlocks/Abilities/AbilityTypes/WarlocksGA_EnemyTarget.h"
#include "WarlocksGA_LightningBolt.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksGA_LightningBolt : public UWarlocksGA_EnemyTarget
{
	GENERATED_BODY()

public:
	UWarlocksGA_LightningBolt();

	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
											   FGameplayTag ApplicationTag) override;

	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData) override;

private:
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* OnCastParticle;

	UPROPERTY(EditDefaultsOnly)
	float Range;
};
