// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarlocksGameMode.h"
#include "WarlocksPlayerController.h"
#include "WarlocksCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AWarlocksGameMode::AWarlocksGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AWarlocksPlayerController::StaticClass();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SafeZone", Actors);

	if (!Actors.IsEmpty())
	{
		if (const auto Actor = Cast<AStaticMeshActor>(Actors[0]))
		{
			SafeZone = Actor;
		}
	}
}

float AWarlocksGameMode::GetCurrentSafeZoneRadius()
{
	const float Width = SafeZone->GetStaticMeshComponent()->GetStaticMesh()->GetBoundingBox().GetSize().X;
	return Width * CurrentSafeZoneScale / 2;
}
