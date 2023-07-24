#pragma once

#include "CoreMinimal.h"
#include "Warlocks/Abilities/WarlocksGA_GroundTarget.h"
#include "WarlocksGA_Fireball.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksGA_Fireball final : public UWarlocksGA_GroundTarget
{
	GENERATED_BODY()

public:
	UWarlocksGA_Fireball();

	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	                                           FGameplayTag ApplicationTag) override;

	virtual void OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AWarlocksProjectileSpell> ProjectileClass;
};
