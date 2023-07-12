#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarlocksSpell.generated.h"

UENUM()
enum class ETarget { None, Self, Floor, Player };

UENUM()
enum class ESpellType { Area, Buff, Debuff };

UENUM()
enum class ESpellCastType { Instant, Casted, Channel };

UCLASS(Abstract)
class WARLOCKS_API AWarlocksSpell : public AActor
{
	GENERATED_BODY()
	
public:
	AWarlocksSpell();
	
	// numerical stats
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Cooldown = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell,
		meta = (EditCondition = "SpellCastType != ESpellCastType::Instant"))
	float CastTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Duration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Power;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Knockback;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Range;

	// categories

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	ETarget TargetingMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	ESpellType SpellType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	ESpellCastType SpellCastType;

	// meta data

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	FString SpellName;
};
