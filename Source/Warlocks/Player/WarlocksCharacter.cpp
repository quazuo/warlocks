#include "WarlocksCharacter.h"

#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Warlocks/Game/WarlocksGameMode.h"
#include "WarlocksPlayerController.h"
#include "WarlocksPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Warlocks/Game/WarlocksGameState.h"

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

	// multiplayer stuff
	bReplicates = true;
	Super::SetReplicateMovement(true);
}

void AWarlocksCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWarlocksCharacter, Health);
	DOREPLIFETIME(AWarlocksCharacter, MaxHealth);
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
			TakeDamage(GameMode->LavaTickDamage, FDamageEvent(), nullptr, this);
		}
	}
}

void AWarlocksCharacter::Launch(const FVector Direction, const float Force)
{
	if (!GetController()) return;

	GetController()->StopMovement();
	LaunchCharacter(Direction * Force, false, false);
}

void AWarlocksCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AWarlocksCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ApplyItems();
}

float AWarlocksCharacter::TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent,
                                     AController* EventInstigator, AActor* DamageCauser)
{
	const auto State = Cast<AWarlocksPlayerState>(GetPlayerState());
	if (!State || State->bIsDead) return 0;

	const auto AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= AppliedDamage;
	if (Health <= 0)
	{
		Health = 0;
		ServerDie();
	}

	return AppliedDamage;
}

float AWarlocksCharacter::RestoreHealth(const float HealAmount)
{
	const auto State = Cast<AWarlocksPlayerState>(GetPlayerState());
	if (!State || State->bIsDead) return 0;

	const auto PreviousHealth = Health;
	
	Health += HealAmount;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}

	return Health - PreviousHealth;
}

void AWarlocksCharacter::ServerDie_Implementation()
{
	const auto State = Cast<AWarlocksPlayerState>(GetPlayerState());
	if (!State) return;
	
	State->bIsDead = true;
	State->bIsStunned = true;
	State->bIsCastingSpell = false;
	State->bIsChannelingSpell = false;
	SetActorEnableCollision(false);

	const auto CharacterController = Cast<AWarlocksPlayerController>(GetController());
	if (!CharacterController) return;

	CharacterController->StopMovement();
	State->bIsDead = true;

	const auto GameMode = Cast<AWarlocksGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode) return;
	
	const auto GameState = GameMode->GetGameState<AWarlocksGameState>();
	if (GameState)
	{
		const FString Announcement = CharacterController->GetHumanReadableName().Append(" has been slain");
		GameState->Announce(Announcement);
	}
}

void AWarlocksCharacter::ApplyItems()
{
	const auto State = Cast<AWarlocksPlayerState>(GetPlayerState());
	if (!State) return;

	for (const auto &Item : State->Inventory)
	{
		if (Item.CharacterModifier)
			Item.CharacterModifier(this);
	}
}
