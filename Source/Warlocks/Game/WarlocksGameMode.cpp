#include "WarlocksGameMode.h"

#include "WarlocksGameState.h"
#include "Actors/WarlocksSafeZone.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Player/WarlocksPlayerState.h"

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

	if (!SafeZone)
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
	AWarlocksPlayerState* WinnerState = nullptr;

	// TickLavaDamage(DeltaSeconds);

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


void AWarlocksGameMode::TickLavaDamage(const float DeltaTime) const
{
	const auto SafeZoneRadius = SafeZone->GetCurrentSafeZoneRadius();

	for (const auto &PlayerState : GameState->PlayerArray)
	{
		const auto MyPlayerState = Cast<AWarlocksPlayerState>(PlayerState);
		const auto Warlock = Cast<AWarlocksCharacter>(PlayerState->GetPawn());
		if (!MyPlayerState || !Warlock) continue;

		const auto Location = Warlock->GetActorLocation();

		if (Location.Length() > SafeZoneRadius)
		{
			MyPlayerState->ApplyDamage(DeltaTime * LavaDamage);
		}
	}
}

void AWarlocksGameMode::ResetPlayers()
{
	UE_LOG(LogGameMode, Error, TEXT("Restarting players..."));
	
	for (const auto &Player : GameState->PlayerArray)
	{
		const auto WarlocksState = Cast<AWarlocksPlayerState>(Player);
		if (WarlocksState)
		{
			WarlocksState->Reset();
			
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

void AWarlocksGameMode::EndRound(AWarlocksPlayerState* WinnerState)
{
	UE_LOG(LogGameMode, Error, TEXT("Ending round..."));

	const auto State = GetGameState<AWarlocksGameState>();
	if (State)
	{
		const FString Announcement = FString::Printf(TEXT("Player %d has won the round!"), WinnerState->GetPlayerId());
		State->Announce(Announcement);
	}
	
	const auto Warlock = Cast<AWarlocksCharacter>(WinnerState->GetPawn());
	if (Warlock)
	{
		//WinnerState->bIsVictorious = true;
		//WinnerState->bIsStunned = true;
		Warlock->GetController()->StopMovement();

		// rotate towards the camera (forwards)
		const FVector Vec = {-1, 0, 0};
		Warlock->SetActorRotation(Vec.Rotation());
	}
	
	GetWorldTimerManager().ClearTimer(SafeZoneTimer);

	FTimerDelegate RoundTransitionDelegate;
	RoundTransitionDelegate.BindUFunction(this, FName("StartRound"));
	GetWorldTimerManager().SetTimer(RoundTransitionTimer, RoundTransitionDelegate, RoundTransitionTime, false);
}
