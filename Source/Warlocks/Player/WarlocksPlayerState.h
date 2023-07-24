#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Warlocks/Warlocks.h"
#include "WarlocksPlayerState.generated.h"

class UGameplayAbility;
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

	UFUNCTION(BlueprintCallable)
	UWarlocksAttributeSet* GetAttributeSet() const;
	
	void Stun();

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UGameplayAbility> GetAbilityClass(const ESpell SpellSlot) const;

private:
	UPROPERTY()
	UWarlocksAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UWarlocksAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> MoveToAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> QStartupAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> WStartupAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> EStartupAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> RStartupAbility;

	FGameplayAbilitySpec QAbilitySpec, WAbilitySpec, EAbilitySpec, RAbilitySpec;

	FGameplayAbilitySpec MakeStartingAbilitySpec(const TSubclassOf<UGameplayAbility>& StartupAbility,
	                                             EWarlocksAbilityInputID AbilityInputID);

	void AddStartingAbilities();
};
