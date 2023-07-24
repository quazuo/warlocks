#include "WarlocksPlayerController.h"

#include "AbilitySystemComponent.h"
#include "WarlocksCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "WarlocksPlayerState.h"

AWarlocksPlayerController::AWarlocksPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void AWarlocksPlayerController::HandleMoveToPressed()
{
	if (const auto State = GetPlayerState<AWarlocksPlayerState>())
	{
		const auto ASC = State->GetAbilitySystemComponent();
		
		for (const auto& Spec : ASC->GetActivatableAbilities())
		{
			if (Spec.InputID != static_cast<uint32>(EWarlocksAbilityInputID::MoveTo))
				continue;
			if (!Spec.Ability || !Spec.IsActive())
				continue;

			ASC->CancelAbilityHandle(Spec.Handle);
		}
	}
	
	SendLocalInputToASC(true, EWarlocksAbilityInputID::MoveTo);
}

void AWarlocksPlayerController::HandleMoveToReleased()
{
	SendLocalInputToASC(false, EWarlocksAbilityInputID::MoveTo);
}

void AWarlocksPlayerController::SendLocalInputToASC(const bool bIsPressed,
                                                    const EWarlocksAbilityInputID AbilityInputID)
{
	const auto State = Cast<AWarlocksPlayerState>(PlayerState);
	if (!State || !State->GetAbilitySystemComponent())
		return;
	
	if (bIsPressed)
	{
		State->GetAbilitySystemComponent()->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		State->GetAbilitySystemComponent()->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

void AWarlocksPlayerController::DoDebugThing()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetCharacter());
	if (Warlock)
	{
		Warlock->Launch(FVector::ForwardVector, 1000);
	}
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
		EnhancedInputComponent->BindAction(MoveToAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::HandleMoveToPressed);
		EnhancedInputComponent->BindAction(MoveToAction, ETriggerEvent::Completed, this,
										   &AWarlocksPlayerController::HandleMoveToReleased);

		EnhancedInputComponent->BindAction(AbilityQAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::HandleAbilityQPressed);
		EnhancedInputComponent->BindAction(AbilityQAction, ETriggerEvent::Completed, this,
										   &AWarlocksPlayerController::HandleAbilityQReleased);

		EnhancedInputComponent->BindAction(AbilityWAction, ETriggerEvent::Started, this,
										   &AWarlocksPlayerController::HandleAbilityWPressed);
		EnhancedInputComponent->BindAction(AbilityWAction, ETriggerEvent::Completed, this,
										   &AWarlocksPlayerController::HandleAbilityWReleased);

		EnhancedInputComponent->BindAction(AbilityEAction, ETriggerEvent::Started, this,
										   &AWarlocksPlayerController::HandleAbilityEPressed);
		EnhancedInputComponent->BindAction(AbilityEAction, ETriggerEvent::Completed, this,
										   &AWarlocksPlayerController::HandleAbilityEReleased);

		EnhancedInputComponent->BindAction(AbilityRAction, ETriggerEvent::Started, this,
										   &AWarlocksPlayerController::HandleAbilityRPressed);
		EnhancedInputComponent->BindAction(AbilityRAction, ETriggerEvent::Completed, this,
										   &AWarlocksPlayerController::HandleAbilityRReleased);

		EnhancedInputComponent->BindAction(DebugAction, ETriggerEvent::Started, this,
		                                   &AWarlocksPlayerController::DoDebugThing);
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
