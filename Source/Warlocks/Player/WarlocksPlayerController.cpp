#include "WarlocksPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "WarlocksCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WarlocksPlayerState.h"
#include "Camera/CameraActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Spells/WarlocksProjectileSpell.h"

AWarlocksPlayerController::AWarlocksPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
}

void AWarlocksPlayerController::DoDebugThing()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (Warlock)
	{
		Warlock->Launch(FVector::ForwardVector, 1000);
	}
}

float AWarlocksPlayerController::GetRemainingCooldown(ESpell SpellSlot)
{
	const auto CooldownTimer = GetCooldownTimer(SpellSlot);
	return GetSpellClass(SpellSlot) && CooldownTimer->IsValid()
		       ? GetWorldTimerManager().GetTimerRemaining(*CooldownTimer)
		       : 0;
}

float AWarlocksPlayerController::GetRemainingCooldownPercent(ESpell SpellSlot)
{
	const auto SpellClass = GetSpellClass(SpellSlot);
	const auto CooldownTimer = GetCooldownTimer(SpellSlot);

	if (SpellClass && CooldownTimer->IsValid())
	{
		const auto RemainingCooldown = GetWorldTimerManager().GetTimerRemaining(*CooldownTimer);

		const auto SpellInstance = SpellClass.GetDefaultObject();
		if (!SpellInstance) return 0;

		return RemainingCooldown / SpellInstance->Cooldown;
	}

	return 0;
}

float AWarlocksPlayerController::GetRemainingCastTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(SpellCastTimer);
}

float AWarlocksPlayerController::GetRemainingCastTimePercent() const
{
	if (CurrentlyCastedSpell && SpellCastTimer.IsValid())
	{
		const auto RemainingCastTime = GetWorldTimerManager().GetTimerRemaining(SpellCastTimer);

		const auto SpellInstance = CurrentlyCastedSpell.GetDefaultObject();
		if (!SpellInstance) return 0;

		return RemainingCastTime / SpellInstance->CastTime;
	}

	return 0;
}

void AWarlocksPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AWarlocksPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::OnMoveInputStarted);

		// spell cast events
		EnhancedInputComponent->BindAction(QSpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartQSpellCast);
		EnhancedInputComponent->BindAction(WSpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartWSpellCast);
		EnhancedInputComponent->BindAction(ESpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartESpellCast);
		EnhancedInputComponent->BindAction(RSpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartRSpellCast);

		// debug event
		EnhancedInputComponent->BindAction(DebugAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::DoDebugThing);
	}
}

void AWarlocksPlayerController::OnMoveInputStarted()
{
	const auto State = Cast<AWarlocksPlayerState>(PlayerState);
	if (!State || State->bIsStunned) return;

	if (GetWorldTimerManager().GetTimerRemaining(SpellCastTimer) > 0) return;

	StopChannelingSpell();
	StopMovement();

	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		return;
	
	ServerMoveTo(Hit.Location);
	CachedDestination = Hit.Location;

	// this is needed because of the problem that occurs when we call SimpleMoveToLocation with a respawned pawn
	if (GetLocalRole() < ROLE_Authority)
	{
		UPathFollowingComponent* PathFollowingComp = FindComponentByClass<UPathFollowingComponent>();
		if (PathFollowingComp)
		{
			PathFollowingComp->UpdateCachedComponents();
			bUpdatedMovementCache = true;
			UE_LOG(LogActor, Error, TEXT("Updated!"));
		}
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator,
	                                               FVector(1), true, true, ENCPoolMethod::None, true);
}

void AWarlocksPlayerController::ServerMoveTo_Implementation(const FVector Destination)
{
	StopMovement();
	CachedDestination = Destination;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
}

void AWarlocksPlayerController::StartSpellCast(ESpell SpellSlot)
{
	// check if we can cast the spell
	if (!GetWorld()) return;
	if (GetRemainingCooldown(SpellSlot) > 0) return;
	if (CurrentlyCastedSpell) return;

	// check if warlock is spawned and is alive
	const auto State = Cast<AWarlocksPlayerState>(PlayerState);
	if (!State || State->bIsStunned) return;

	// check if there is a spell in the slot
	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	const auto SpellInstance = SpellClass.GetDefaultObject();

	// if warlock was knocked back and hasn't landed yet, only instant spells can be cast [subject for change]
	ACharacter* ControlledCharacter = GetCharacter();
	if (SpellInstance->SpellCastType != ESpellCastType::Instant
		&& ControlledCharacter->GetCharacterMovement()->MovementMode == MOVE_Falling)
		return;

	// get location of cursor and rotator pointing to it, so we can rotate the character in that direction.
	// if it cannot be found and the spell needs it (i.e. it's floor targeted) return immediately.
	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit) && SpellInstance->TargetingMode == ETarget::Floor) return;

	const FVector CharacterLoc = ControlledCharacter->GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	StopMovement();

	FVector CursorLocation = Hit.Location;
	CursorLocation.SetComponentForAxis(EAxis::Z, CharacterLoc.Z);
	const FVector CursorDirection = CursorLocation - CharacterLoc;
	Rotation = CursorDirection.Rotation();
	RotateCharacter(Rotation);

	StopChannelingSpell();

	if (SpellInstance->SpellCastType == ESpellCastType::Instant)
	{
		CastSpell(SpellSlot, CharacterLoc, Rotation);
	}
	else
	{
		State->bIsCastingSpell = true;
		CurrentlyCastedSpell = SpellClass;

		FTimerDelegate SpellCastDelegate;
		SpellCastDelegate.BindUFunction(this, FName("CastSpell"), SpellSlot, CharacterLoc, Rotation);
		GetWorldTimerManager().SetTimer(SpellCastTimer, SpellCastDelegate, SpellInstance->CastTime, false);
	}
}

void AWarlocksPlayerController::ApplyItemsToSpell(AWarlocksSpell* Spell) const
{
	const auto State = Cast<AWarlocksPlayerState>(PlayerState);
	if (!State) return;

	for (const auto& Item : State->Inventory)
	{
		if (Item.SpellModifier)
			Item.SpellModifier(Spell);
	}
}

AWarlocksSpell* AWarlocksPlayerController::SpawnTempSpell(UClass* Class, FVector const& Location,
                                                          FRotator const& Rotation) const
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetCharacter();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const auto Spell = GetWorld()->SpawnActor<AWarlocksSpell>(Class, Location, Rotation);
	if (!Spell) return nullptr;

	ApplyItemsToSpell(Spell);
	return Spell;
}

void AWarlocksPlayerController::ServerSpawnSpell_Implementation(UClass* Class, FVector const& Location,
                                                                FRotator const& Rotation) const
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetCharacter();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UE_LOG(LogActor, Error, TEXT("Server: Owner: %s"),
	       SpawnParams.Owner
	       ? *SpawnParams.Owner->GetHumanReadableName()
	       : TEXT("null"));

	const auto Spell = GetWorld()->SpawnActor<AWarlocksSpell>(Class, Location, Rotation, SpawnParams);
	if (!Spell) return;

	UE_LOG(LogActor, Error, TEXT("Spell not null"));

	ApplyItemsToSpell(Spell);
}

void AWarlocksPlayerController::CastSpell(ESpell SpellSlot, const FVector Location, const FRotator Rotation)
{
	CurrentlyCastedSpell = nullptr;

	const auto State = Cast<AWarlocksPlayerState>(PlayerState);
	if (!State || State->bIsStunned) return;

	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	const auto SpellInstance = SpellClass.GetDefaultObject();

	State->bIsCastingSpell = false;
	StartSpellCooldown(SpellSlot);

	// spawn the spell actor(s)
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetCharacter();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UE_LOG(LogActor, Error, TEXT("Client: Owner: %s"),
	       SpawnParams.Owner
	       ? *SpawnParams.Owner->GetHumanReadableName()
	       : TEXT("null"));

	// if the spell is of type `Projectile`, 
	const auto ProjectileSpellInstance = Cast<AWarlocksProjectileSpell>(SpellInstance);
	if (ProjectileSpellInstance)
	{
		// spawn temporary spell and apply items to see modified stats like projectile count and such
		const auto TempSpell = Cast<AWarlocksProjectileSpell>(SpawnTempSpell(SpellClass, Location, Rotation));
		if (!TempSpell) return;

		// spawn multiple projectiles spread around
		TArray<FRotator> Rotations = FWarlocksUtils::GetSpreadRotators(Rotation, TempSpell->ProjectileCount,
		                                                               TempSpell->ProjectileSpread);
		TempSpell->Destroy();

		for (const auto& R : Rotations)
		{
			ServerSpawnSpell(SpellClass, Location, R); // todo - merge into 1 rpc
		}
	}
	else
	{
		// if it's not of type `Projectile`, just spawn it in
		ServerSpawnSpell(SpellClass, Location, Rotation);

		if (SpellClass.GetDefaultObject()->SpellCastType == ESpellCastType::Channel)
		{
			// CurrentlyChanneledSpell = Spell;
			State->bIsChannelingSpell = true;
		}
	}
}

void AWarlocksPlayerController::RotateCharacter(const FRotator& Rotation)
{
	const auto ControlledCharacter = GetCharacter();

	ControlledCharacter->bUseControllerRotationPitch = true;
	ControlledCharacter->bUseControllerRotationYaw = true;

	SetControlRotation(Rotation);
	ControlledCharacter->FaceRotation(Rotation);

	ControlledCharacter->bUseControllerRotationPitch = false;
	ControlledCharacter->bUseControllerRotationYaw = false;
}

void AWarlocksPlayerController::StartSpellCooldown(ESpell SpellSlot)
{
	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	const auto CooldownTimer = GetCooldownTimer(SpellSlot);

	GetWorldTimerManager().SetTimer(*CooldownTimer, []
	{
	}, SpellClass.GetDefaultObject()->Cooldown, false);
}

void AWarlocksPlayerController::StopChannelingSpell()
{
	const auto State = Cast<AWarlocksPlayerState>(PlayerState);
	if (!State) return;

	if (CurrentlyChanneledSpell)
	{
		CurrentlyChanneledSpell->Destroy();
		CurrentlyChanneledSpell = nullptr;
		State->bIsChannelingSpell = false;
	}
}
