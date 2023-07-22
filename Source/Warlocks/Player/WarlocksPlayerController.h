#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Spells/WarlocksSpell.h"
#include "WarlocksPlayerController.generated.h"

class AWarlocksCharacter;
class UNiagaraSystem;
class AWarlocksSpell;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class ESpell : uint8
{
	SpellQ UMETA(DisplayName = "Q Spell"),
	SpellW UMETA(DisplayName = "W Spell"),
	SpellE UMETA(DisplayName = "E Spell"),
	SpellR UMETA(DisplayName = "R Spell"),
};

USTRUCT(BlueprintType)
struct FSpellSlot
{
	GENERATED_BODY()

	TSubclassOf<AWarlocksSpell> SpellClass;

	FTimerHandle CooldownTimer;

	FORCEINLINE AWarlocksSpell* GetSpellCDO() const
	{
		if (!SpellClass) return nullptr;
		return SpellClass.GetDefaultObject();
	}
};

#define GAS_INPUT_HANDLERS(Name) \
	FORCEINLINE void Handle##Name##Pressed() { SendLocalInputToASC(true, EWarlocksAbilityInputID::##Name##); } \
	FORCEINLINE void Handle##Name##Released() { SendLocalInputToASC(false, EWarlocksAbilityInputID::##Name##); }

UCLASS()
class AWarlocksPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWarlocksPlayerController();

	// fx for click-to-move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

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

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

private:
	void HandleMoveToPressed();
	void HandleMoveToReleased();
	
	//GAS_INPUT_HANDLERS(MoveTo);
	GAS_INPUT_HANDLERS(AbilityQ);
	GAS_INPUT_HANDLERS(AbilityW);
	GAS_INPUT_HANDLERS(AbilityE);
	GAS_INPUT_HANDLERS(AbilityR);
	
	void SendLocalInputToASC(const bool bIsPressed, const EWarlocksAbilityInputID AbilityInputID) const;

	UFUNCTION()
	void RotateCharacter(const FRotator& Rotation);
};
