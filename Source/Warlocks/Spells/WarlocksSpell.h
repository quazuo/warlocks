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
	float Cooldown = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats,
		meta = (EditCondition = "SpellCastType != ESpellCastType::Instant"))
	float CastTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats,
		meta = (EditCondition = "SpellCastType == ESpellCastType::Channel"))
	float ChannelTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats,
		meta = (EditCondition = "SpellType == ESpellType::Projectile"))
	float ProjectileSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats,
		meta = (EditCondition = "SpellType == ESpellType::Projectile"))
	int ProjectileCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats,
		meta = (EditCondition = "SpellType == ESpellType::Projectile"))
	int ProjectileSpread = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats,
		meta = (EditCondition = "SpellType == ESpellType::Projectile"))
	float ProjectileHitboxRadius = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float Power;

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
