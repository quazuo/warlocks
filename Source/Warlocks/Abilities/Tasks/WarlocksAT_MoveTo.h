#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WarlocksAT_MoveTo.generated.h"

class AWarlocksPlayerController;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMoveToDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class WARLOCKS_API UWarlocksAT_MoveTo final : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FMoveToDelegate OnFinish;

	virtual void Activate() override;

	virtual void TickTask(const float DeltaTime) override;

	UFUNCTION()
	static UWarlocksAT_MoveTo* MoveTo(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
	                                  AWarlocksPlayerController* Controller, const FVector Location,
	                                  const uint8 Priority = 196);

	static UWarlocksAT_MoveTo* MoveTo(IGameplayTaskOwnerInterface& InTaskOwner, AWarlocksPlayerController* Controller,
	                                  const FVector Location, const uint8 Priority = 196);

private:
	void OnMoveFinish();

	FVector InitialLocation;
	FVector TargetLocation;

	UPROPERTY()
	AWarlocksPlayerController* Controller;
};
