#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "WarlocksCharacter.generated.h"

class UAbilitySystemComponent;
class UWarlocksAbilitySystemComponent;

UCLASS(Blueprintable)
class AWarlocksCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	explicit AWarlocksCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	TWeakObjectPtr<UWarlocksAbilitySystemComponent> AbilitySystemComponent;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	UFUNCTION()
	void ApplyKnockback(FVector Direction, const float Force);

private:
	void BindAbilitiesToInput(UInputComponent* PlayerInputComponent) const;
};
