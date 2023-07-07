// Fill out your copyright notice in the Description page of Project Settings.


#include "WarlocksMeditate.h"

#include "AssetViewWidgets.h"
#include "Warlocks/Player/WarlocksCharacter.h"

AWarlocksMeditate::AWarlocksMeditate()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	RootComponent = ContinuousParticle;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(0, 0.53, 0.29));
	PointLight->SetupAttachment(RootComponent);

	// enable ticking because this spell heals the caster every tick
	PrimaryActorTick.bCanEverTick = true;
}

void AWarlocksMeditate::BeginPlay()
{
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;
	
	RootComponent->AttachToComponent(Warlock->GetRootComponent(), { EAttachmentRule::SnapToTarget, false });

	SetLifeSpan(Duration);
	
	Super::BeginPlay();
}

void AWarlocksMeditate::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (const auto Warlock = Cast<AWarlocksCharacter>(GetOwner()))
	{
		Warlock->bIsChannelingSpell = false;
	}
	
	Super::EndPlay(EndPlayReason);
}

void AWarlocksMeditate::Tick(float DeltaTime)
{
	if (const auto Warlock = Cast<AWarlocksCharacter>(GetOwner()))
	{
		Warlock->RestoreHealth(Power);
	}

	Super::Tick(DeltaTime);
}