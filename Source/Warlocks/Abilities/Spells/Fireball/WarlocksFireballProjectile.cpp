#include "WarlocksFireballProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Warlocks/Player/WarlocksCharacter.h"

AWarlocksFireballProjectile::AWarlocksFireballProjectile()
{
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetIntensity(1000);
	PointLight->SetLightColor(FLinearColor(1, 0.5, 0));
	PointLight->SetupAttachment(RootComponent);

	ContinuousParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ContinuousParticle"));
	ContinuousParticle->SetupAttachment(RootComponent);
}

void AWarlocksFireballProjectile::OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                              const FHitResult& SweepResult)
{
	Super::OnHit(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (OtherActor == GetInstigator() || Cast<AWarlocksSpellActor>(OtherActor))
		return;
	
	const auto SourceWarlock = Cast<AWarlocksCharacter>(GetInstigator());
	if (!SourceWarlock) return;
	
	if (const auto TargetWarlock = Cast<AWarlocksCharacter>(OtherActor))
	{
		if (const auto TargetASC = Cast<UWarlocksAbilitySystemComponent>(TargetWarlock->GetAbilitySystemComponent()))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
	
		TargetWarlock->Launch(-1 * SweepResult.Normal, Knockback);
	}
	
	SpawnOnHitParticle();
	Destroy();
}
