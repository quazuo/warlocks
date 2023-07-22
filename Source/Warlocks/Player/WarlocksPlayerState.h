#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Warlocks/Abilities/Spells/WarlocksGA_Fireball.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/WarlocksGA_MoveTo.h"
#include "WarlocksPlayerState.generated.h"

class UWarlocksGameplayAbility;
class UWarlocksAttributeSet;
class UWarlocksAbilitySystemComponent;

UCLASS(BlueprintType)
class WARLOCKS_API AWarlocksPlayerState final : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWarlocksPlayerState();

	virtual void BeginPlay() override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UWarlocksAttributeSet* GetAttributeSet() const;
	
	void Stun();

private:
	UPROPERTY()
	UWarlocksAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UWarlocksAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UWarlocksGameplayAbility> MoveToAbility = UWarlocksGA_MoveTo::StaticClass();

	const TSubclassOf<UWarlocksGameplayAbility> QStartupAbility = UWarlocksGA_Fireball::StaticClass();
	const TSubclassOf<UWarlocksGameplayAbility> WStartupAbility = UWarlocksGA_Fireball::StaticClass();
	const TSubclassOf<UWarlocksGameplayAbility> EStartupAbility = UWarlocksGA_Fireball::StaticClass();
	const TSubclassOf<UWarlocksGameplayAbility> RStartupAbility = UWarlocksGA_Fireball::StaticClass();

	FGameplayAbilitySpec GetStartingAbilitySpec(const TSubclassOf<UWarlocksGameplayAbility>& StartupAbility,
	                                            EWarlocksAbilityInputID AbilityInputID);

	void AddStartingAbilities();
};
