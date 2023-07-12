// Fill out your copyright notice in the Description page of Project Settings.


#include "WarlocksFireball.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

AWarlocksFireball::AWarlocksFireball()
{
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(1, 0.5, 0));
	PointLight->SetupAttachment(RootComponent);

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);
}

void AWarlocksFireball::BeginPlay()
{
	Super::BeginPlay();
}

void AWarlocksFireball::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarlocksFireball::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	if (OtherActor == this || OtherActor == GetOwner() || !GetOwner() || Cast<AWarlocksSpell>(OtherActor)) return;

	UE_LOG(LogActor, Error, TEXT("OnHit: Owner: %s"),
	GetOwner()
		? *GetOwner()->GetHumanReadableName()
		: TEXT("null"));

	UE_LOG(LogActor, Error, TEXT("OnHit: OtherActor: %s"),
	OtherActor
		? *OtherActor->GetHumanReadableName()
		: TEXT("null"));
	
	if (const auto Enemy = Cast<AWarlocksCharacter>(OtherActor))
	{
		const auto OwnerWarlock = Cast<AWarlocksCharacter>(GetOwner());
		const auto OwnerController = OwnerWarlock ? OwnerWarlock->GetController() : nullptr;
		UGameplayStatics::ApplyDamage(Enemy, Power, OwnerController, this, nullptr);

		UE_LOG(LogActor, Error, TEXT("Fireball hit: %f damage"), Power);

		Enemy->Launch(-1 * SweepResult.Normal, Knockback);
		
		if (const auto Controller = Cast<AWarlocksPlayerController>(Enemy->GetController()))
		{
			Controller->StopChannelingSpell();
		}
	}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitParticle, GetActorLocation(),
		FRotator::ZeroRotator, FVector(.3));
	Destroy();
}
