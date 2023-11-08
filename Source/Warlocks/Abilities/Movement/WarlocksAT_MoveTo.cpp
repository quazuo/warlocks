#include "WarlocksAT_MoveTo.h"

#include "NavigationPath.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

void UWarlocksAT_MoveTo::Activate()
{
	Super::Activate();

	if (const auto Character = Controller->GetCharacter())
	{
		InitialLocation = Character->GetActorLocation();
	}

	bTickingTask = true;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(Controller, TargetLocation);
}

void UWarlocksAT_MoveTo::TickTask(const float DeltaTime)
{
	Super::TickTask(DeltaTime);

	const auto Character = Controller->GetCharacter();
	const auto CharMovement = Character ? Character->GetCharacterMovement() : nullptr;
	if (!CharMovement)
		return;

	FVector DistanceLeft = Character->GetActorLocation() - TargetLocation;
	DistanceLeft.Z = 0;

	FVector DistanceTraversed = Character->GetActorLocation() - InitialLocation;
	DistanceTraversed.Z = 0;
	const bool bStoppedNotTrivially = !DistanceTraversed.IsNearlyZero()
		&& CharMovement->Velocity.X == 0
		&& CharMovement->Velocity.Y == 0;

	const bool bIsCurrentPath = !UAIBlueprintHelperLibrary::GetCurrentPathPoints(Controller).IsEmpty();

	if (bIsCurrentPath || DistanceLeft.IsNearlyZero(1) || bStoppedNotTrivially)
	{
		OnMoveFinish();
	}
}

UWarlocksAT_MoveTo* UWarlocksAT_MoveTo::MoveTo(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
                                               AWarlocksPlayerController* Controller, const FVector Location,
                                               const uint8 Priority)
{
	UWarlocksAT_MoveTo* MyTask = NewTaskUninitialized<UWarlocksAT_MoveTo>();
	if (MyTask && TaskOwner.GetInterface())
	{
		MyTask->InitTask(*TaskOwner, Priority);
		MyTask->Controller = Controller;
		MyTask->TargetLocation = Location;
	}
	return MyTask;
}

UWarlocksAT_MoveTo* UWarlocksAT_MoveTo::MoveTo(IGameplayTaskOwnerInterface& InTaskOwner,
                                               AWarlocksPlayerController* Controller, const FVector Location,
                                               const uint8 Priority)
{
	if (!Controller)
		return nullptr;

	UWarlocksAT_MoveTo* MyTask = NewTaskUninitialized<UWarlocksAT_MoveTo>();
	if (MyTask)
	{
		MyTask->InitTask(InTaskOwner, Priority);
		MyTask->Controller = Controller;
		MyTask->TargetLocation = Location;
	}
	return MyTask;
}

void UWarlocksAT_MoveTo::OnMoveFinish()
{
	OnFinish.Broadcast(FGameplayTag(), FGameplayEventData());
	EndTask();
}
