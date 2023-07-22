#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WarlocksAT_CastSpell.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpellCastDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

UCLASS()
class WARLOCKS_API UWarlocksAT_CastSpell final : public UAbilityTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSpellCastDelegate OnFinish;

	virtual void Activate() override;

	UFUNCTION()
	static UWarlocksAT_CastSpell* TaskCastSpell(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
	                                            const float CastTime, const uint8 Priority = 192);

	static UWarlocksAT_CastSpell* TaskCastSpell(IGameplayTaskOwnerInterface& InTaskOwner, const float CastTime,
	                                            const uint8 Priority = FGameplayTasks::DefaultPriority);

	float GetPassedTime() const;

	float GetPassedTimePercent() const;

private:
	void OnTimeFinish();

	float CastTime = 0.f;
	FTimerHandle TimerHandle;
};
