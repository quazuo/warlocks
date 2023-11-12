#include "WarlocksCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Warlocks/Game/WarlocksGameMode.h"
#include "WarlocksPlayerState.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Movement/WarlocksCharacterMoveComponent.h"

AWarlocksCharacter::AWarlocksCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UWarlocksCharacterMoveComponent>(
		CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42, 96);

	// don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// tick stuff: uncomment when adding tick function
	// PrimaryActorTick.bCanEverTick = true;
	// PrimaryActorTick.bStartWithTickEnabled = true;

	// replication stuff
	bReplicates = true;
	Super::SetReplicateMovement(true);
}

void AWarlocksCharacter::ApplyKnockback(FVector Direction, const float Force)
{
	Direction.Z = 0;
	Direction.Normalize();
	
	if (GetController())
	{
		GetController()->StopMovement();
	}

	LaunchCharacter(Direction * Force, false, false);

	if (const auto State = Cast<AWarlocksPlayerState>(GetPlayerState()))
	{
		State->ApplyStun();
	}
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
	{
		UE_LOG(LogWarlocks, Error, TEXT("BindAbilitiesToInput(): no PlayerInputComponent"));
		return;
	}

	if (!AbilitySystemComponent.IsValid())
	{
		UE_LOG(LogWarlocks, Error, TEXT("BindAbilitiesToInput(): no valid AbilitySystemComponent"));
		return;
	}

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
