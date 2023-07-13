// Fill out your copyright notice in the Description page of Project Settings.


#include "WarlocksMeditate.h"

#include "AssetViewWidgets.h"
#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Player/WarlocksPlayerState.h"

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

TSubclassOf<UObject> AWarlocksMeditate::GetBPClassPtr()
{
	const auto ObjPath =
		TEXT("/Script/Engine.Blueprint'/Game/Warlocks/Blueprints/Spells/BP_WarlocksMeditate.BP_WarlocksMeditate'");
	return FWarlocksUtils::GetBPClassPtr(ObjPath);
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
		if (const auto State = Cast<AWarlocksPlayerState>(Warlock->GetPlayerState()))
		{
			State->bIsChannelingSpell = false;
		} 
	}
	
	Super::EndPlay(EndPlayReason);
}

void AWarlocksMeditate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	const auto Warlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!Warlock) return;

	Warlock->RestoreHealth(Power);

	const auto State = Cast<AWarlocksPlayerState>(Warlock->GetPlayerState());
	if (!State) return;

	if (!State->bIsChannelingSpell)
		Destroy();
}