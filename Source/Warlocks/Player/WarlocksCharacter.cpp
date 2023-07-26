#include "WarlocksCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Warlocks/Game/WarlocksGameMode.h"
#include "WarlocksPlayerState.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"

AWarlocksCharacter::AWarlocksCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42, 96);

	// don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// configure character movement
	const auto CharMove = GetCharacterMovement();
	CharMove->bOrientRotationToMovement = true; // Rotate character to moving direction
	CharMove->RotationRate = FRotator(0, 640, 0);
	CharMove->bConstrainToPlane = true;
	CharMove->bSnapToPlaneAtStart = true;
	CharMove->MaxAcceleration = 100000;
	CharMove->BrakingDecelerationWalking = 100000;
	CharMove->GravityScale = 0; // don't stop falling until velocity == 0
	CharMove->BrakingDecelerationFalling = 200;

	// tick stuff
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// replication stuff
	bReplicates = true;
	Super::SetReplicateMovement(true);
}

void AWarlocksCharacter::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// check if character should stop falling, if so change mode to walking
	const auto CharMovement = GetCharacterMovement();
	if (CharMovement->MovementMode == MOVE_Falling && CharMovement->Velocity.X == 0 && CharMovement->Velocity.Y == 0)
	{
		CharMovement->SetMovementMode(MOVE_Walking);

		if (const auto MyController = GetController())
		{
			MyController->StopMovement();
		}
	}

	// apply damage if standing on lava
	if (const auto GameMode = Cast<AWarlocksGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		const auto SafeZoneRadius = GameMode->GetCurrentSafeZoneRadius();
		const auto MyLocation = GetActorLocation();

		if (pow(MyLocation.X, 2) + pow(MyLocation.Y, 2) > pow(SafeZoneRadius, 2))
		{
			TakeDamage(GameMode->LavaDamage * DeltaSeconds, FDamageEvent(), nullptr, this);
		}
	}
}

void AWarlocksCharacter::Launch(const FVector Direction, const float Force)
{
	if (!GetController()) return;
	GetController()->StopMovement();
	LaunchCharacter(Direction * Force, false, false);
}

void AWarlocksCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (const auto State = GetPlayerState<AWarlocksPlayerState>())
	{
		AbilitySystemComponent = Cast<UWarlocksAbilitySystemComponent>(State->GetAbilitySystemComponent());

		// AI won't have PlayerControllers so we can init again here just to be sure.
		// No harm in initing twice for heroes that have PlayerControllers.
		State->GetAbilitySystemComponent()->InitAbilityActorInfo(State, this);
	}
}

void AWarlocksCharacter::BindAbilitiesToInput(UInputComponent* PlayerInputComponent) const
{
	if (!PlayerInputComponent)
		return;
	
	// Bind to AbilitySystemComponent
	const FTopLevelAssetPath AbilityEnumAssetPath = FTopLevelAssetPath(
		FName("/Script/Warlocks"),
		FName("EWarlocksAbilityInputID")
	);
	
	const auto BindInfo = FGameplayAbilityInputBinds(
		FString("ConfirmTarget"),
		FString("CancelTarget"),
		AbilityEnumAssetPath,
		static_cast<int32>(EWarlocksAbilityInputID::Confirm),
		static_cast<int32>(EWarlocksAbilityInputID::Cancel)
	);
	
	AbilitySystemComponent->BindAbilityActivationToInputComponent(PlayerInputComponent, BindInfo);
}

void AWarlocksCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (const auto State = GetPlayerState<AWarlocksPlayerState>())
	{
		AbilitySystemComponent = Cast<UWarlocksAbilitySystemComponent>(State->GetAbilitySystemComponent());

		// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
		State->GetAbilitySystemComponent()->InitAbilityActorInfo(State, this);
	}

	BindAbilitiesToInput(InputComponent);
}

void AWarlocksCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	BindAbilitiesToInput(PlayerInputComponent);
}

UAbilitySystemComponent* AWarlocksCharacter::GetAbilitySystemComponent() const
{
	const auto State = Cast<AWarlocksPlayerState>(GetPlayerState());
	if (!State) return nullptr;
	return State->GetAbilitySystemComponent();
}
