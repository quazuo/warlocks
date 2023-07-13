#pragma once

#include "CoreMinimal.h"
#include "WarlocksProjectileSpell.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "WarlocksFireball.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksFireball final : public AWarlocksProjectileSpell
{
	GENERATED_BODY()

public:
	AWarlocksFireball();

	static TSubclassOf<UObject> GetBPClassPtr();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	virtual void SpawnOnHitParticle() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	UPointLightComponent* PointLight;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* OnHitParticle;
};
