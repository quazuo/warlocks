#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Warlocks/Warlocks.h"
#include "WarlocksPlayerController.generated.h"

class AWarlocksCharacter;
class UNiagaraSystem;
class AWarlocksSpell;
class UInputAction;
class UInputMappingContext;

#define GAS_INPUT_HANDLERS(Name) \
	FORCEINLINE void Handle##Name##Pressed() { SendLocalInputToASC(true, EWarlocksAbilityInputID::##Name##); } \
	FORCEINLINE void Handle##Name##Released() { SendLocalInputToASC(false, EWarlocksAbilityInputID::##Name##); }

UCLASS()
class AWarlocksPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWarlocksPlayerController();

	// mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true")) \
	UInputAction* MoveToAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true")) \
	UInputAction* AbilityQAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true")) \
	UInputAction* AbilityWAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true")) \
	UInputAction* AbilityEAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true")) \
	UInputAction* AbilityRAction;

	// actions

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* DebugAction;

	/** debug thing */
	UFUNCTION()
	void DoDebugThing();
	
	void RotateCharacter(const FRotator& Rotation);

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

private:
	void HandleMoveToPressed();
	void HandleMoveToReleased();
	
	GAS_INPUT_HANDLERS(AbilityQ);
	GAS_INPUT_HANDLERS(AbilityW);
	GAS_INPUT_HANDLERS(AbilityE);
	GAS_INPUT_HANDLERS(AbilityR);
	
	void SendLocalInputToASC(const bool bIsPressed, const EWarlocksAbilityInputID AbilityInputID);
};
