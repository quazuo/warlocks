// Fill out your copyright notice in the Description page of Project Settings.


#include "WarlocksFlash.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Player/WarlocksCharacter.h"

AWarlocksFlash::AWarlocksFlash()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpellSceneComponent"));
}

void AWarlocksFlash::BeginPlay()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;

	// spawn initial particle
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
											 FRotator::ZeroRotator, FVector(.25));

	// move caster
	Warlock->bIsChannelingSpell = false;
	Warlock->GetController()->StopMovement();
	Warlock->GetCharacterMovement()->Velocity = FVector::Zero();
	Warlock->SetActorLocation(Warlock->GetActorLocation() + GetActorRotation().Vector() * Range);

	// spawn next particle
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
											 FRotator::ZeroRotator, FVector(.25));
	
	Super::BeginPlay();
}

void AWarlocksFlash::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AWarlocksFlash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
