// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "WarlocksGameMode.generated.h"

UCLASS(minimalapi)
class AWarlocksGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWarlocksGameMode();

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LavaTickDamage = 0.08;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentSafeZoneScale = 30;

	UFUNCTION()
	float GetCurrentSafeZoneRadius();

private:
	UPROPERTY()
	AStaticMeshActor* SafeZone;
};



