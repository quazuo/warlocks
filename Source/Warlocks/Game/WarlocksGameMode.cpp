#include "WarlocksGameMode.h"

#include "WarlocksGameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Player/WarlocksPlayerState.h"
#include "Actors/WarlocksSafeZone.h"

AWarlocksGameMode::AWarlocksGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// use our custom PlayerController class
	PlayerControllerClass = AWarlocksPlayerController::StaticClass();
	PlayerStateClass = AWarlocksPlayerState::StaticClass();

	// find the safe zone actor
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWarlocksSafeZone::StaticClass(), Actors);
	if (!Actors.IsEmpty())
	{
		SafeZone = Cast<AWarlocksSafeZone>(Actors[0]);
	}
	else
	{
		UE_LOG(LogWarlocks, Error, TEXT("No SafeZone actor could be found"));
	}
}

void AWarlocksGameMode::BeginPlay()
{
	Super::BeginPlay();
	StartRound();
}

void AWarlocksGameMode::Tick(float DeltaSeconds)
{
	int AlivePlayerCount = 0;
	const AWarlocksPlayerState* WinnerState = nullptr;

	for (const auto &Player : GameState->PlayerArray)
	{
		// const auto WarlocksState = Cast<AWarlocksPlayerState>(Player);
		// if (WarlocksState && !WarlocksState->bIsDead)
		// {
		// 	AlivePlayerCount++;
		// 	WinnerState = WarlocksState;
		// }
	}

	if (!bIsRoundTransition && GameState->PlayerArray.Num() > 1 && AlivePlayerCount == 1 && WinnerState)
	{
		bIsRoundTransition = true;
		EndRound(WinnerState);
	}
}

void AWarlocksGameMode::ResetPlayers()
{
	UE_LOG(LogGameMode, Error, TEXT("Restarting players..."));
	
	for (const auto &Player : GameState->PlayerArray)
	{
		if (const auto WarlocksState = Cast<AWarlocksPlayerState>(Player))
		{
			const auto Warlock = Cast<AWarlocksCharacter>(WarlocksState->GetPawn());
			const auto CharacterController = Cast<AWarlocksPlayerController>(Warlock->GetController());

			CharacterController->GetPawn()->Destroy();
			RestartPlayer(CharacterController);
		}
	}
}

void AWarlocksGameMode::StartRound()
{
	UE_LOG(LogGameMode, Error, TEXT("Starting round..."));
	bIsRoundTransition = false;
	SafeZone->ResetSafeZone();
	ResetPlayers();
}

void AWarlocksGameMode::EndRound(const AWarlocksPlayerState* WinnerState)
{
	UE_LOG(LogGameMode, Error, TEXT("Ending round..."));
	
	if (const auto State = GetGameState<AWarlocksGameState>())
	{
		const FText PlayerName = FText::FromString(WinnerState->GetPlayerName());
		State->GetAnnouncer()->AnnouncePlayerDeath(PlayerName);
	}
	
	if (const auto Warlock = Cast<AWarlocksCharacter>(WinnerState->GetPawn()))
	{
		//WinnerState->bIsVictorious = true;
		//WinnerState->bIsStunned = true;
		Warlock->GetController()->StopMovement();

		// rotate towards the camera (forwards)
		const FVector Vec = {-1, 0, 0};
		Warlock->SetActorRotation(Vec.Rotation());
	}

	FTimerDelegate RoundTransitionDelegate;
	RoundTransitionDelegate.BindUFunction(this, FName("StartRound"));
	GetWorldTimerManager().SetTimer(RoundTransitionTimer, RoundTransitionDelegate, RoundTransitionTime, false);
}
