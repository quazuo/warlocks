// Copyright Epic Games, Inc. All Rights Reserved.

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
#include "Spells/WarlocksSpell.h"

AWarlocksPlayerController::AWarlocksPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
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

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);
	if (!Cameras.IsEmpty())
	{
		SetViewTargetWithBlend(Cameras.Last());
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
		                                   &AWarlocksPlayerController::OnInputStarted);

		// spell cast events
		EnhancedInputComponent->BindAction(QSpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartQSpellCast);
		EnhancedInputComponent->BindAction(WSpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartWSpellCast);
		EnhancedInputComponent->BindAction(ESpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartESpellCast);
		EnhancedInputComponent->BindAction(RSpellCastAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::StartRSpellCast);
	}
}

void AWarlocksPlayerController::OnInputStarted()
{
	if (GetCharacter()->GetCharacterMovement()->MovementMode == MOVE_Falling) return;
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
	UWorld* World = GetWorld();
	if (!World) return;
	
	if (GetRemainingCooldown(SpellSlot) > 0) return;
	if (CurrentlyCastedSpell) return;

	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	const auto SpellInstance = SpellClass.GetDefaultObject();

	ACharacter* ControlledCharacter = GetCharacter();
	if (SpellInstance->SpellCastType != ESpellCastType::Instant
		&& ControlledCharacter->GetCharacterMovement()->MovementMode == MOVE_Falling) return;

	StopMovement();

	const FVector CharacterLoc = ControlledCharacter->GetActorLocation();
	FRotator Rotation = FRotator::ZeroRotator;

	if (SpellInstance->TargetingMode == ETarget::Floor)
	{
		FHitResult Hit;
		if (!GetHitResultUnderCursor(ECC_Visibility, true, Hit)) return;

		FVector CursorLocation = Hit.Location;
		CursorLocation.SetComponentForAxis(EAxis::Z, CharacterLoc.Z);
		const FVector CursorDirection = CursorLocation - CharacterLoc;
		Rotation = CursorDirection.Rotation();

		RotateCharacter(Rotation);
	}

	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock) return;

	StopChannelingSpell();
	
	if (SpellInstance->SpellCastType == ESpellCastType::Instant)
	{
		CastSpell(SpellSlot, CharacterLoc, Rotation);
	}
	else
	{
		Warlock->StartCastingSpell();
		CurrentlyCastedSpell = SpellClass;

		FTimerDelegate SpellCastDelegate;
		SpellCastDelegate.BindUFunction(this, FName("CastSpell"), SpellSlot, CharacterLoc, Rotation);
		GetWorldTimerManager().SetTimer(SpellCastTimer, SpellCastDelegate, SpellInstance->CastTime, false);
	}
}

void AWarlocksPlayerController::CastSpell(ESpell SpellSlot, const FVector Location, const FRotator Rotation)
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock) return;

	Warlock->StopCastingSpell();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetCharacter();
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	const auto SpellClass = GetSpellClass(SpellSlot);
	if (!SpellClass) return;

	AWarlocksSpell* Spell = GetWorld()->SpawnActor<AWarlocksSpell>(
		SpellClass, Location, Rotation, SpawnParams);
	if (!Spell) return;

	if (SpellClass.GetDefaultObject()->SpellCastType == ESpellCastType::Channel)
	{
		CurrentlyChanneledSpell = Spell;
		Warlock->StartChannelingSpell();
	}

	CurrentlyCastedSpell = nullptr;
	StartSpellCooldown(SpellSlot);
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

	auto CooldownTimer = GetCooldownTimer(SpellSlot);
	
	GetWorldTimerManager().SetTimer(*CooldownTimer, []{}, SpellClass.GetDefaultObject()->Cooldown, false);
}

void AWarlocksPlayerController::StopChannelingSpell()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (!Warlock) return;

	if (CurrentlyChanneledSpell)
	{
		CurrentlyChanneledSpell->Destroy();
		CurrentlyChanneledSpell = nullptr;
		Warlock->StopChannelingSpell();
	}
}
