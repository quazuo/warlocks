#pragma once

#include "CoreMinimal.h"
#include "WarlocksProjectileSpell.h"

#include "WarlocksFireball.generated.h"

class UParticleSystemComponent;
class UPointLightComponent;

UCLASS()
class WARLOCKS_API AWarlocksFireball final : public AWarlocksProjectileSpell
{
	GENERATED_BODY()

public:
	AWarlocksFireball();
	
	static TSubclassOf<UObject> GetBPClassPtr();
	
	virtual void SpawnOnHitParticle() override;

private:
	UPROPERTY(EditAnywhere, Category = Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UPointLightComponent* PointLight;
	
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* OnHitParticle;
};
