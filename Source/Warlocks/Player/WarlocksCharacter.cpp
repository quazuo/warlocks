#include "WarlocksCharacter.h"

#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Warlocks/Game/WarlocksGameMode.h"
#include "WarlocksPlayerController.h"
#include "WarlocksPlayerState.h"

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
	if (bIsDead) return 0;

	const auto AppliedDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= AppliedDamage;
	if (Health <= 0)
	{
		Health = 0;
		Die();
	}

	return AppliedDamage;
}

float AWarlocksCharacter::RestoreHealth(const float HealAmount)
{
	if (bIsDead) return 0;

	const auto PreviousHealth = Health;
	
	Health += HealAmount;
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}

	return Health - PreviousHealth;
}

void AWarlocksCharacter::Die()
{
	bIsDead = true;
	bIsStunned = true;
	bIsCastingSpell = false;
	bIsChannelingSpell = false;
	SetActorEnableCollision(false);

	const auto CharacterController = Cast<AWarlocksPlayerController>(GetController());
	if (!CharacterController) return;

	const auto State = Cast<AWarlocksPlayerState>(GetPlayerState());
	if (!State) return;

	const auto GameMode = Cast<AWarlocksGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GameMode) return;

	CharacterController->StopMovement();
	State->bIsDead = true;
	
	const FString Announcement = CharacterController->PlayerName.Append(" has been slain");
	GameMode->Announce(Announcement);
}

void AWarlocksCharacter::Refresh()
{
	const auto CDO = Cast<AWarlocksCharacter>(StaticClass()->GetDefaultObject());
	if (!CDO) return;

	bIsStunned = CDO->bIsStunned;
	bIsCastingSpell = CDO->bIsCastingSpell;
	bIsChannelingSpell = CDO->bIsChannelingSpell;
	bIsVictorious = CDO->bIsVictorious;
	MaxHealth = CDO->MaxHealth;
	Health = MaxHealth;
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
