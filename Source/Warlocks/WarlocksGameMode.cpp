// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarlocksGameMode.h"
#include "WarlocksPlayerController.h"
#include "WarlocksCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWarlocksGameMode::AWarlocksGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AWarlocksPlayerController::StaticClass();
}