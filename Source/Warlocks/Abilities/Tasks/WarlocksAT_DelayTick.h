#pragma once

#include "WarlocksAT_Delay.h"
#include "WarlocksAT_DelayTick.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDelayTickDelegate, float, DeltaTime);

UCLASS()
class UWarlocksAT_DelayTick : public UWarlocksAT_Delay
{
	GENERATED_BODY()

public:
	UWarlocksAT_DelayTick();

	UPROPERTY(BlueprintAssignable)
	FDelayTickDelegate OnTick;

	UFUNCTION()
	static UWarlocksAT_DelayTick* TaskDelayTick(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
	                                            const float TotalTime, const uint8 Priority = 192);

protected:
	virtual void TickTask(float DeltaTime) override;
};
