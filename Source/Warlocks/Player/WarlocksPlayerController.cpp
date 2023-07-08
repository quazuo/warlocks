#include "WarlocksPlayerController.h"

#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "WarlocksCharacter.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// TArray<AActor*> Cameras;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);
	// if (!Cameras.IsEmpty())
	// {
	// 	SetViewTargetWithBlend(Cameras.Last());
	// }
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
	if (!GetCharacter()) return;

	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock) return;

	if (Warlock->bIsStunned) return;
	if (GetWorldTimerManager().GetTimerRemaining(SpellCastTimer) > 0) return;

	StopChannelingSpell();
	StopMovement();

	FHitResult Hit;
	if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		CachedDestination = Hit.Location;
	}

	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination,
	                                               FRotator::ZeroRotator, FVector(1), true, true,
	                                               ENCPoolMethod::None, true);
}

void AWarlocksPlayerController::StartSpellCast(ESpell SpellSlot)
{
	// check if we can cast the spell
	if (!GetWorld()) return;
	if (GetRemainingCooldown(SpellSlot) > 0) return;
	if (CurrentlyCastedSpell) return;

	// check if character is spawned and is alive
	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock || Warlock->bIsStunned) return;

	// check if there is a spell in the slot
	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	const auto SpellInstance = SpellClass.GetDefaultObject();

	// if warlock was knocked back and hasn't landed yet, only instant spells can be cast [subject for change]
	ACharacter* ControlledCharacter = GetCharacter();
	if (SpellInstance->SpellCastType != ESpellCastType::Instant
		&& ControlledCharacter->GetCharacterMovement()->MovementMode == MOVE_Falling)
		return;

	StopMovement();

	const FVector CharacterLoc = ControlledCharacter->GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	// get location of cursor and rotator pointing to it, so we can rotate the character in that direction.
	// if it cannot be found and the spell needs it (i.e. it's floor targeted) return immediately.
	FHitResult Hit;
	if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit) && SpellInstance->TargetingMode == ETarget::Floor) return;

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
		Warlock->bIsCastingSpell = true;
		CurrentlyCastedSpell = SpellClass;

		FTimerDelegate SpellCastDelegate;
		SpellCastDelegate.BindUFunction(this, FName("CastSpell"), SpellSlot, CharacterLoc, Rotation);
		GetWorldTimerManager().SetTimer(SpellCastTimer, SpellCastDelegate, SpellInstance->CastTime, false);
	}
}

void AWarlocksPlayerController::CastSpell(ESpell SpellSlot, const FVector Location, const FRotator Rotation)
{
	CurrentlyCastedSpell = nullptr;

	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock || Warlock->bIsStunned) return;

	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	const auto SpellInstance = SpellClass.GetDefaultObject();

	Warlock->bIsCastingSpell = false;
	StartSpellCooldown(SpellSlot);

	// spawn the spell actor(s)
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetCharacter();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// if the spell is of type `Projectile`, 
	const auto ProjectileSpellInstance = Cast<AWarlocksProjectileSpell>(SpellInstance);
	if (ProjectileSpellInstance)
	{
		// spawn multiple projectiles spread around
		TArray<FRotator> Rotations = FWarlocksUtils::GetSpreadRotators(Rotation, ProjectileSpellInstance->ProjectileCount,
		                                                              ProjectileSpellInstance->ProjectileSpread);

		for (const auto& R : Rotations)
		{
			const AWarlocksProjectileSpell* Spell = GetWorld()->SpawnActor<AWarlocksProjectileSpell>(
				SpellClass, Location, R, SpawnParams);
		}
	}
	else
	{
		// if it's not of type `Projectile`, just spawn it in
		AWarlocksSpell* Spell = GetWorld()->SpawnActor<AWarlocksSpell>(
			SpellClass, Location, Rotation, SpawnParams);

		if (SpellClass.GetDefaultObject()->SpellCastType == ESpellCastType::Channel)
		{
			CurrentlyChanneledSpell = Spell;
			Warlock->bIsChannelingSpell = true;
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
	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock) return;

	if (CurrentlyChanneledSpell)
	{
		CurrentlyChanneledSpell->Destroy();
		CurrentlyChanneledSpell = nullptr;
		Warlock->bIsChannelingSpell = false;
	}
}
