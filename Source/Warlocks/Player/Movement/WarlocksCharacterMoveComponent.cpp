#include "WarlocksCharacterMoveComponent.h"

#include "Warlocks/Player/WarlocksPlayerState.h"

UWarlocksCharacterMoveComponent::UWarlocksCharacterMoveComponent()
{
	bOrientRotationToMovement = true; // Rotate character to moving direction
	RotationRate = FRotator(0, 640, 0);
	bConstrainToPlane = true;
	bSnapToPlaneAtStart = true;
	GravityScale = 0; // don't stop falling until horizontal velocity is zero
	BrakingDecelerationFalling = 200;
	bRequestedMoveUseAcceleration = false;
}

void UWarlocksCharacterMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovementMode == MOVE_Falling && Velocity.X == 0 && Velocity.Y == 0)
	{
		SetMovementMode(MOVE_Walking);

		if (const auto MyController = GetController())
		{
			MyController->StopMovement();

			if (const auto State = MyController->GetPlayerState<AWarlocksPlayerState>())
			{
				State->RemoveStun();
			}
		}
	}
}
