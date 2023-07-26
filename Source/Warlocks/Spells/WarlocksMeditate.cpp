#include "WarlocksMeditate.h"

#include "AssetViewWidgets.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Warlocks/Warlocks.h"

AWarlocksMeditate::AWarlocksMeditate()
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

void AWarlocksMeditate::BeginPlay()
{
	Super::BeginPlay();
	
	if (const auto Warlock = GetOwner())
	{
		UE_LOG(LogWarlocks, Error, TEXT("%s"), *Warlock->GetActorNameOrLabel());
		RootComponent->AttachToComponent(Warlock->GetRootComponent(), { EAttachmentRule::SnapToTarget, false });
	}
}
