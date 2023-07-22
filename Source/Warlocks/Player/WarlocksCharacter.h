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
	AWarlocksCharacter();

protected:
	virtual void Tick(float DeltaSeconds) override;
	
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION()
	void Launch(FVector Direction, const float Force);

	TWeakObjectPtr<UWarlocksAbilitySystemComponent> AbilitySystemComponent;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	void BindAbilitiesToInput(UInputComponent* PlayerInputComponent) const;
};
