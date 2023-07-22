#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarlocksGameplayAbility.generated.h"

UCLASS(Abstract)
class WARLOCKS_API UWarlocksGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

private:
	bool bActivateAbilityOnGranted = false;
};
