#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarlocksSpell.generated.h"

UENUM()
enum class ETargetingMode { None, Self, Floor, Player };

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

	/** Time which needs to pass between this spell's casts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Cooldown = 1;

	/** Time it takes to cast this spell */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell,
		meta = (EditCondition = "SpellCastType != ESpellCastType::Instant"))
	float CastTime = 1;

	/** The duration of this spell, which might mean various things based on the spell */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Duration;

	/** The power of this spell, which mostly means the damage or healing it applies per second */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Power;

	/** The knockback of this spell, which currently applies only within OnHit callbacks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Knockback;

	/** [unimplemented] The maximum range at which the spell can be cast, or the maximum distance a projectile can fly */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	float Range;

	// spell categories

	/** [partly unimplemented] The way this spell chooses its target, currently only (kinda) supporting Floor spellcasts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	ETargetingMode TargetingMode;

	/** [unimplemented] This spell's type */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	ESpellType SpellType;

	/** The way this spell is cast */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	ESpellCastType SpellCastType;

	// meta data

	/** This spell's name, specifically the key which will be used to lookup metadata in the spells' data table */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Spell)
	FString SpellName;
};
