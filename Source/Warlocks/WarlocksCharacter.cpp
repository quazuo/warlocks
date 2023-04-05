// Copyright Epic Games, Inc. All Rights Reserved.

#include "WarlocksCharacter.h"

#include "WarlocksGameMode.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AWarlocksCharacter::AWarlocksCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42, 96);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	const auto CharMove = GetCharacterMovement();
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

void AWarlocksCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// check if character should stop falling, if so change mode to walking
	const auto CharMovement = GetCharacterMovement();
	if (CharMovement->MovementMode == MOVE_Falling && CharMovement->Velocity.X == 0 && CharMovement->Velocity.Y == 0)
	{
		CharMovement->SetMovementMode(MOVE_Walking);
	}

	// apply damage if standing on lava
	if (const auto GameMode = Cast<AWarlocksGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		const auto SafeZoneRadius = GameMode->GetCurrentSafeZoneRadius();
		const auto MyLocation = GetActorLocation();
		
		if (pow(MyLocation.X, 2) + pow(MyLocation.Y, 2) > pow(SafeZoneRadius, 2))
		{
			ModifyHealth(-1 * GameMode->LavaTickDamage);
		}
	}
}

void AWarlocksCharacter::Launch(const FVector Direction)
{
	if (!GetController()) return;

	GetController()->StopMovement();
	LaunchCharacter(Direction * 500, false, false);
}

void AWarlocksCharacter::ModifyHealth(const float Value)
{
	if (bIsDead) return;
	
	Health += Value;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
	else if (Health <= 0)
	{
		Health = 0;
		Die();
	}
}

void AWarlocksCharacter::Die()
{
	bIsDead = true;
	StopCastingSpell();
	StopChannelingSpell();
	SetActorEnableCollision(false);
	if (const auto Controller = GetController())
	{
		Controller->StopMovement();
	}
	
	// todo - probably some more behavior, perhaps letting the GameMode know we died or something
}
