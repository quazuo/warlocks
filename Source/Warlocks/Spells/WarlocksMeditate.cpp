#include "WarlocksMeditate.h"

#include "AssetViewWidgets.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"

AWarlocksMeditate::AWarlocksMeditate()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(0, 0.53, 0.29));
	PointLight->SetupAttachment(RootComponent);

	// enable ticking because this spell heals the caster every tick
	PrimaryActorTick.bCanEverTick = true;
}

void AWarlocksMeditate::BeginPlay()
{
	const auto Warlock = GetOwner();
	if (!Warlock) return;
	
	RootComponent->AttachToComponent(Warlock->GetRootComponent(), { EAttachmentRule::SnapToTarget, false });
	SetLifeSpan(Duration);
	Super::BeginPlay();
}
