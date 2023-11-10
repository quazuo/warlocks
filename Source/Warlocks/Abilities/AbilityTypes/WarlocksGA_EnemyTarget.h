#pragma once

#include "CoreMinimal.h"
#include "WarlocksGA_ClientToServer.h"
#include "WarlocksGA_EnemyTarget.generated.h"

class AWarlocksCharacter;
UCLASS()
class WARLOCKS_API UWarlocksGA_EnemyTarget : public UWarlocksGA_ClientToServer
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
											   FGameplayTag ApplicationTag) override;

	virtual void ActivateLocalPlayerAbility(const FGameplayAbilitySpecHandle Handle,
											const FGameplayAbilityActorInfo* ActorInfo,
											const FGameplayAbilityActivationInfo ActivationInfo,
											const FGameplayEventData* TriggerEventData) override;

	UPROPERTY()
	AWarlocksCharacter* CachedTarget;
};
