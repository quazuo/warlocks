#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

	// actions
	
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

	/** debug thing */
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

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FSpellSlot GetSpellSlot(const ESpell SpellSlot) const
	{
		switch (SpellSlot)
		{
		case ESpell::SpellQ: return QSpellSlot;
		case ESpell::SpellW: return WSpellSlot;
		case ESpell::SpellE: return ESpellSlot;
		case ESpell::SpellR: return RSpellSlot;
		default: unimplemented();
			return {};
		}
	}

	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWarlocksSpell* GetSpellCDO(const ESpell Slot) const
	{
		const auto SpellSlot = GetSpellSlot(Slot);
		return SpellSlot.GetSpellCDO();
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

protected:
	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

private:
	FVector CachedDestination;
	
	FSpellSlot QSpellSlot, WSpellSlot, ESpellSlot, RSpellSlot;

	FTimerHandle SpellCastTimer;

	FORCEINLINE FTimerHandle* GetCooldownTimer(const ESpell SpellSlot)
	{
		switch (SpellSlot)
		{
		case ESpell::SpellQ: return &QSpellSlot.CooldownTimer;
		case ESpell::SpellW: return &WSpellSlot.CooldownTimer;
		case ESpell::SpellE: return &ESpellSlot.CooldownTimer;
		case ESpell::SpellR: return &RSpellSlot.CooldownTimer;
		default: unimplemented();
			return nullptr;
		}
	}

	UFUNCTION()
	void OnMoveInputStarted();

	UFUNCTION(Server, Reliable)
	void ServerMoveTo(const FVector Destination);

	UFUNCTION()
	void StartSpellCast(ESpell SpellSlot);

	UFUNCTION(Server, Reliable)
	void ServerPreSpellCast(FRotator Rotation);

	UFUNCTION()
	void RotateCharacter(const FRotator& Rotation);

	UFUNCTION()
	void StartSpellCooldown(ESpell SpellSlot);

	UFUNCTION()
	void CastSpell(ESpell SpellSlot, const FVector Location, const FRotator Rotation);

	UFUNCTION()
	void ApplyItemsToSpell(AWarlocksSpell* Spell) const;

	UFUNCTION(Server, Reliable)
	void ServerSpawnSpell(UClass* Class, FVector const& Location, FRotator const& Rotation) const;

	UFUNCTION()
	AWarlocksSpell* SpawnTempSpell(UClass* Class, FVector const& Location, FRotator const& Rotation) const;
};
