#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "Warlocks/Spells/WarlocksSpell.h"
#include "WarlocksPlayerController.generated.h"

class AWarlocksCharacter;
/** Forward declaration to improve compiling times */
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

UCLASS()
class AWarlocksPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWarlocksPlayerController();

	// FX for click-to-move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	// Mapping context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* QSpellCastAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* WSpellCastAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ESpellCastAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* RSpellCastAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* DebugAction;

	// debug thing
	UFUNCTION()
	void DoDebugThing();

	// dummy spell-casting wrapper UFunctions; used by actions
	UFUNCTION()
	FORCEINLINE void StartQSpellCast() { StartSpellCast(ESpell::SpellQ); }

	UFUNCTION()
	FORCEINLINE void StartWSpellCast() { StartSpellCast(ESpell::SpellW); }

	UFUNCTION()
	FORCEINLINE void StartESpellCast() { StartSpellCast(ESpell::SpellE); }

	UFUNCTION()
	FORCEINLINE void StartRSpellCast() { StartSpellCast(ESpell::SpellR); }

	// classes of spells in each of the slots
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	TSubclassOf<AWarlocksSpell> QSpellClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	TSubclassOf<AWarlocksSpell> WSpellClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	TSubclassOf<AWarlocksSpell> ESpellClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	TSubclassOf<AWarlocksSpell> RSpellClass;

	// Spell wrappers
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TSubclassOf<AWarlocksSpell> GetSpellClass(ESpell SpellSlot) const
	{
		switch (SpellSlot)
		{
		case ESpell::SpellQ: return QSpellClass;
		case ESpell::SpellW: return WSpellClass;
		case ESpell::SpellE: return ESpellClass;
		case ESpell::SpellR: return RSpellClass;
		default: unimplemented();
			return nullptr;
		}
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWarlocksSpell* GetSpellCDO(ESpell SpellSlot) const
	{
		const auto SpellClass = GetSpellClass(SpellSlot);
		if (!SpellClass) return nullptr;
		return SpellClass.GetDefaultObject();
	}

	// Spell utils
	UFUNCTION(BlueprintCallable, Category = Spell)
	float GetRemainingCooldown(ESpell SpellSlot);

	UFUNCTION(BlueprintCallable, Category = Spell)
	float GetRemainingCooldownPercent(ESpell SpellSlot);

	UFUNCTION(BlueprintCallable, Category = Spell)
	float GetRemainingCastTime() const;

	UFUNCTION(BlueprintCallable, Category = Spell)
	float GetRemainingCastTimePercent() const;

	UFUNCTION()
	void StopChannelingSpell();

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	void OnMoveInputStarted();

	UFUNCTION(Server, Reliable)
	void ServerMoveTo(const FVector Destination);

	UFUNCTION()
	void StartSpellCast(ESpell SpellSlot);

private:
	FVector CachedDestination;

	FTimerHandle SpellCastTimer;
	FTimerHandle QSpellCooldownTimer, WSpellCooldownTimer, ESpellCooldownTimer, RSpellCooldownTimer;

	FORCEINLINE FTimerHandle* GetCooldownTimer(ESpell SpellSlot)
	{
		switch (SpellSlot)
		{
		case ESpell::SpellQ: return &QSpellCooldownTimer;
		case ESpell::SpellW: return &WSpellCooldownTimer;
		case ESpell::SpellE: return &ESpellCooldownTimer;
		case ESpell::SpellR: return &RSpellCooldownTimer;
		default: unimplemented();
			return nullptr;
		}
	}

	TSubclassOf<AWarlocksSpell> CurrentlyCastedSpell;

	UPROPERTY()
	AWarlocksSpell* CurrentlyChanneledSpell;

	void RotateCharacter(const FRotator& Rotation);

	void StartSpellCooldown(ESpell SpellSlot);

	UFUNCTION()
	void CastSpell(ESpell SpellSlot, const FVector Location, const FRotator Rotation);

	UFUNCTION()
	void ApplyItemsToSpell(AWarlocksSpell *Spell) const;

	UFUNCTION(Server, Reliable)
	void ServerSpawnSpell(UClass* Class, FVector const& Location, FRotator const& Rotation) const;

	UFUNCTION()
	AWarlocksSpell* SpawnTempSpell(UClass* Class, FVector const& Location, FRotator const& Rotation) const;
};
