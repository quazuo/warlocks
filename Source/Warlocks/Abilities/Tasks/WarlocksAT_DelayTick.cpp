#include "WarlocksAT_DelayTick.h"

UWarlocksAT_DelayTick::UWarlocksAT_DelayTick()
{
	bTickingTask = true;
}

UWarlocksAT_DelayTick* UWarlocksAT_DelayTick::TaskDelayTick(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
                                                            const float TotalTime, const uint8 Priority)
{
	UWarlocksAT_DelayTick* MyTask = NewTaskUninitialized<UWarlocksAT_DelayTick>();
	if (MyTask && TaskOwner.GetInterface())
	{
		MyTask->InitTask(*TaskOwner, Priority);
		MyTask->TotalTime = TotalTime;
	}
	return MyTask;
}

void UWarlocksAT_DelayTick::TickTask(float DeltaTime)
{
	OnTick.Broadcast(DeltaTime);
}
