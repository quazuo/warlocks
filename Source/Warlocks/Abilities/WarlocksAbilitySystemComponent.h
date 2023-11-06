#pragma once

#include "AbilitySystemComponent.h"
#include "WarlocksAbilitySystemComponent.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksAbilitySystemComponent final : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	bool bStartupAbilitiesGiven = false;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class UWarlocksAT_Delay> SpellCastTask;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<class UWarlocksAT_DelayTick> ChannelTask;

	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
};
