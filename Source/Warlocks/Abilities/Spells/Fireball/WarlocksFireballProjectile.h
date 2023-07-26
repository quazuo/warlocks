#pragma once

#include "Warlocks/Abilities/Spells/WarlocksProjectile.h"
#include "GameplayEffect.h"
#include "WarlocksFireballProjectile.generated.h"

class UParticleSystemComponent;
class UPointLightComponent;

UCLASS()
class WARLOCKS_API AWarlocksFireballProjectile final : public AWarlocksProjectile
{
	GENERATED_BODY()

public:
	AWarlocksFireballProjectile();

	UPROPERTY(BlueprintReadWrite)
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	float Knockback;

private:
	UPROPERTY(EditAnywhere, Category = Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UPointLightComponent* PointLight;
	
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
};
