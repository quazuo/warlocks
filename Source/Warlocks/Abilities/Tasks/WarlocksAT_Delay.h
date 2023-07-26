#pragma once

#include "Abilities/Tasks/AbilityTask.h"
#include "WarlocksAT_Delay.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDelayDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class WARLOCKS_API UWarlocksAT_Delay : public UAbilityTask
{
	GENERATED_BODY()

public:
	UWarlocksAT_Delay();

	UPROPERTY(BlueprintAssignable)
	FDelayDelegate OnFinish;

	virtual void Activate() override;

	UFUNCTION()
	static UWarlocksAT_Delay* TaskDelay(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
	                                    const float TotalTime, const uint8 Priority = 192);

	UFUNCTION(BlueprintCallable)
	float GetPassedTime() const;

	UFUNCTION(BlueprintCallable)
	float GetPassedTimePercent() const;

protected:
	void OnTimeFinish();

	float TotalTime = 0.f;
	FTimerHandle TimerHandle;
};
