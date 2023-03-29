// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarlocksGameMode.h"
#include "WarlocksPlayerController.h"
#include "WarlocksCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWarlocksGameMode::AWarlocksGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AWarlocksPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_WarlocksCharacter"));
	if (PlayerPawnBPClass.Class)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_WarlocksPlayerController"));
	if (PlayerControllerBPClass.Class)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}