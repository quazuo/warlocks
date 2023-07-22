#include "WarlocksStatusEffect.h"

#include "Particles/ParticleSystemComponent.h"

#include "Warlocks/Player/WarlocksCharacter.h"

AWarlocksStatusEffect::AWarlocksStatusEffect()
{
	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	RootComponent = ContinuousParticle;

	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
}

void AWarlocksStatusEffect::BeginPlay()
{
	Super::BeginPlay();

	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;
	
	//Warlock->ActiveEffects.Add(this);
	RootComponent->AttachToComponent(Warlock->GetRootComponent(), { EAttachmentRule::SnapToTarget, false });
	SetLifeSpan(Duration);
}

void AWarlocksStatusEffect::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const auto Warlock = Cast<AWarlocksCharacter>(GetOwner()))
	{
		//Warlock->ActiveEffects.Remove(this);
	}
}
