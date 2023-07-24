#include "WarlocksFlash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Player/WarlocksCharacter.h"

AWarlocksFlash::AWarlocksFlash()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

void AWarlocksFlash::BeginPlay()
{
	Super::BeginPlay();
	
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;

	// spawn initial particle
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
											 FRotator::ZeroRotator, FVector(.25));

	if (Warlock->GetController())
		Warlock->GetController()->StopMovement();
	Warlock->GetCharacterMovement()->Velocity = FVector::Zero();
	Warlock->SetActorLocation(Warlock->GetActorLocation() + GetActorRotation().Vector() * Range);

	// spawn next particle
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
											 FRotator::ZeroRotator, FVector(.25));
}
