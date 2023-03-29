// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarlocksCharacter.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AWarlocksCharacter::AWarlocksCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42, 96);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	auto CharMove = GetCharacterMovement();
	CharMove->bOrientRotationToMovement = true; // Rotate character to moving direction
	CharMove->RotationRate = FRotator(0, 640, 0);
	CharMove->bConstrainToPlane = true;
	CharMove->bSnapToPlaneAtStart = true;
	CharMove->MaxAcceleration = 100000;
	CharMove->BrakingDecelerationWalking = 100000;
	CharMove->GravityScale = 0; // don't stop falling until velocity == 0
	CharMove->BrakingDecelerationFalling = 200;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AWarlocksCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	auto CharMovement = GetCharacterMovement();
	
	if (CharMovement->MovementMode == MOVE_Falling && CharMovement->Velocity.X == 0 && CharMovement->Velocity.Y == 0)
	{
		CharMovement->SetMovementMode(MOVE_Walking);
	}
}

void AWarlocksCharacter::Launch(FVector Direction)
{
	if (!GetController()) return;
	
	GetController()->StopMovement();
	LaunchCharacter(Direction * 500, false, false);
}

void AWarlocksCharacter::ModifyHealth(float Value)
{
	Health += Value;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
	else if (Health <= 0)
	{
		// todo death
	}
}
