#include "WarlocksFireball.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Player/WarlocksCharacter.h"

AWarlocksFireball::AWarlocksFireball()
{
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(1, 0.5, 0));
	PointLight->SetupAttachment(RootComponent);

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);
}

void AWarlocksFireball::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	Super::OnHit(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	// if (OtherActor == this || OtherActor == GetOwner() || Cast<AWarlocksSpell>(OtherActor))
	// 	return;
	//
	// const auto OwnerWarlock = Cast<AWarlocksCharacter>(GetOwner());
	// if (!OwnerWarlock) return;
	//
	// if (const auto Enemy = Cast<AWarlocksCharacter>(OtherActor))
	// {
	// 	// todo - apply damage
	//
	// 	Enemy->Launch(-1 * SweepResult.Normal, Knockback);
	//
	// 	UE_LOG(LogWarlocks, Error, TEXT("Fireball hit: %f damage"), Power);
	// }
	//
	// SpawnOnHitParticle();
	// Destroy();
}

void AWarlocksFireball::SpawnOnHitParticle()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnHitParticle, GetActorLocation(),
		FRotator::ZeroRotator, FVector(.3));
}
