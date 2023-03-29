// Fill out your copyright notice in the Description page of Project Settings.


#include "WarlocksMeditate.h"

#include "AssetViewWidgets.h"
#include "Warlocks/WarlocksCharacter.h"

AWarlocksMeditate::AWarlocksMeditate()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	RootComponent = ContinuousParticle;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(0, 0.53, 0.29));
	PointLight->SetupAttachment(RootComponent);

	PrimaryActorTick.bCanEverTick = true;
	InitialLifeSpan = 10;
}

void AWarlocksMeditate::BeginPlay()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;
	
	RootComponent->AttachToComponent(Warlock->GetRootComponent(), { EAttachmentRule::SnapToTarget, false });
	
	Super::BeginPlay();
}

void AWarlocksMeditate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (Warlock)
	{
		Warlock->StopChannelingSpell();
	}
	
	Super::EndPlay(EndPlayReason);
}

void AWarlocksMeditate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;

	Warlock->ModifyHealth(HealPerTick);
}