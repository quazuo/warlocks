#include "WarlocksAT_CastSpell.h"

UWarlocksAT_CastSpell::UWarlocksAT_CastSpell()
{
	bSimulatedTask = true;
}

UWarlocksAT_CastSpell* UWarlocksAT_CastSpell::TaskCastSpell(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
                                                            const float CastTime, const uint8 Priority)
{
	UWarlocksAT_CastSpell* MyTask = NewTaskUninitialized<UWarlocksAT_CastSpell>();
	if (MyTask && TaskOwner.GetInterface())
	{
		MyTask->InitTask(*TaskOwner, Priority);
		MyTask->CastTime = CastTime;
	}
	return MyTask;
}

UWarlocksAT_CastSpell* UWarlocksAT_CastSpell::TaskCastSpell(IGameplayTaskOwnerInterface& InTaskOwner,
                                                            const float CastTime, const uint8 Priority)
{
	if (CastTime <= 0.f)
		return nullptr;

	UWarlocksAT_CastSpell* MyTask = NewTaskUninitialized<UWarlocksAT_CastSpell>();
	if (MyTask)
	{
		MyTask->InitTask(InTaskOwner, Priority);
		MyTask->CastTime = CastTime;
	}
	return MyTask;
}

float UWarlocksAT_CastSpell::GetPassedTime() const
{
	return GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle);
}

float UWarlocksAT_CastSpell::GetPassedTimePercent() const
{
	const float TotalTime = GetWorld()->GetTimerManager().GetTimerRate(TimerHandle);
	if (TotalTime == 0.f)
		return 0.f;

	return GetWorld()->GetTimerManager().GetTimerElapsed(TimerHandle) / TotalTime;
}

void UWarlocksAT_CastSpell::Activate()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UWarlocksAT_CastSpell::OnTimeFinish, CastTime, false);
}

void UWarlocksAT_CastSpell::OnTimeFinish()
{
	OnFinish.Broadcast(FGameplayTag(), FGameplayEventData());
	EndTask();
}
