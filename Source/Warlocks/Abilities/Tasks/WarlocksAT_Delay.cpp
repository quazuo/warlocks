#include "WarlocksAT_Delay.h"

UWarlocksAT_Delay::UWarlocksAT_Delay()
{
	bSimulatedTask = true;
}

UWarlocksAT_Delay* UWarlocksAT_Delay::TaskDelay(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
                                                            const float TotalTime, const uint8 Priority)
{
	UWarlocksAT_Delay* MyTask = NewTaskUninitialized<UWarlocksAT_Delay>();
	if (MyTask && TaskOwner.GetInterface())
	{
		MyTask->InitTask(*TaskOwner, Priority);
		MyTask->TotalTime = TotalTime;
	}
	return MyTask;
}

float UWarlocksAT_Delay::GetPassedTime() const
{
	return GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle);
}

float UWarlocksAT_Delay::GetPassedTimePercent() const
{
	return GetPassedTime() / TotalTime;
}

void UWarlocksAT_Delay::Activate()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWarlocksAT_Delay::OnTimeFinish, TotalTime, false);
}

void UWarlocksAT_Delay::OnTimeFinish()
{
	OnFinish.Broadcast(FGameplayTag(), FGameplayEventData());
	EndTask();
}
