#pragma once

#include "Warlocks/Abilities/WarlocksGA_GroundTarget.h"
#include "WarlocksGA_Flash.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksGA_Flash final : public UWarlocksGA_GroundTarget
{
	GENERATED_BODY()

public:
	UWarlocksGA_Flash();

	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
											   FGameplayTag ApplicationTag) override;

	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData) override;

private:
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* OnCastParticle;

	UPROPERTY(EditDefaultsOnly)
	float Range;
};
