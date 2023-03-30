// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarlocksSpell.generated.h"

UENUM()
enum class ETarget { None, Self, Floor, Player };

UENUM()
enum class ESpellType { Projectile, Area, Buff, Debuff };

UENUM()
enum class ESpellCastType { Instant, Casted, Channel };

UCLASS(Abstract)
class WARLOCKS_API AWarlocksSpell : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float CastTime;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float ProjectileHitboxRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	ETarget TargetingMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	ESpellType SpellType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	ESpellCastType SpellCastType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Icon)
	UTexture2D* SpellIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Info)
	UStringTable* SpellInfo;
};
