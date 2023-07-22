#include "WarlocksGameMode.h"

#include "WarlocksGameState.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMeshActor.h"

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
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "SafeZone", Actors);

	if (!Actors.IsEmpty())
	{
		if (const auto Actor = Cast<AStaticMeshActor>(Actors[0]))
		{
			SafeZone = Actor;
		}
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

	for (const auto &Player : GameState->PlayerArray)
	{
		const auto WarlocksState = Cast<AWarlocksPlayerState>(Player);
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

float AWarlocksGameMode::GetCurrentSafeZoneRadius() const
{
	const float Width = SafeZone->GetStaticMeshComponent()->GetStaticMesh()->GetBoundingBox().GetSize().X;
	return Width * CurrentSafeZoneScale / 2;
}

void AWarlocksGameMode::ResetSafeZone()
{
	FVector CurrScale = SafeZone->GetStaticMeshComponent()->GetRelativeScale3D();
	
	CurrScale.X = RoundBeginSafeZoneScale;
	CurrScale.Y = CurrScale.X;

	SafeZone->GetStaticMeshComponent()->SetRelativeScale3D(CurrScale);
	CurrentSafeZoneScale = RoundBeginSafeZoneScale;
}

void AWarlocksGameMode::ShrinkSafeZone()
{
	FVector CurrScale = SafeZone->GetStaticMeshComponent()->GetRelativeScale3D();
	
	CurrScale.X -= SafeZoneShrinkDiff;
	if (CurrScale.X < MinSafeZoneScale)
		CurrScale.X = MinSafeZoneScale;
	CurrScale.Y = CurrScale.X;
	
	SafeZone->GetStaticMeshComponent()->SetRelativeScale3D(CurrScale);
	CurrentSafeZoneScale = CurrScale.X;

	const auto State = GetGameState<AWarlocksGameState>();
	if (State)
	{
		State->Announce("The safe zone has shrunk!");
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
	ResetSafeZone();
	ResetPlayers();

	// setup shrinking of the safe zone on an interval
	FTimerDelegate ShrinkDelegate;
	ShrinkDelegate.BindUFunction(this, FName("ShrinkSafeZone"));
	GetWorldTimerManager().ClearTimer(SafeZoneTimer);
	GetWorldTimerManager().SetTimer(SafeZoneTimer, ShrinkDelegate, SafeZoneShrinkInterval, true);
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
