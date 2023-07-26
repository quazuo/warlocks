#include "WarlocksMeditateAura.h"

#include "AssetViewWidgets.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"

AWarlocksMeditateAura::AWarlocksMeditateAura()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(0, 0.53, 0.29));
	PointLight->SetupAttachment(RootComponent);

	bReplicates = true;
}

void AWarlocksMeditateAura::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto Warlock = GetInstigator())
	{
		RootComponent->AttachToComponent(Warlock->GetRootComponent(), { EAttachmentRule::SnapToTarget, false });
	}
}
