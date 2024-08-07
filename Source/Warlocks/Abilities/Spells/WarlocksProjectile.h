#pragma once

#include "WarlocksSpellActor.h"
#include "WarlocksProjectile.generated.h"

UCLASS(Abstract)
class WARLOCKS_API AWarlocksProjectile : public AWarlocksSpellActor
{
	GENERATED_BODY()

public:
	AWarlocksProjectile();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {}
	
public:
	UFUNCTION()
	virtual void SpawnOnHitParticle();
	
	// stats

	/** The speed at which this spell's projectiles travel */
	UPROPERTY(EditAnywhere, Category = Stats)
	float ProjectileSpeed = 1000;

	/** The number of projectiles spawned when this spell is cast */
	UPROPERTY(EditAnywhere, Category = Stats)
	int ProjectileCount = 1;

	/** The angle between spell's projectiles when it's cast */
	UPROPERTY(EditAnywhere, Category = Stats)
	int ProjectileSpread = 10;

	/** The radius of this spell's projectiles' collision spheres */
	UPROPERTY(EditAnywhere, Category = Stats)
	float ProjectileHitboxRadius = 50;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* OnHitParticle;
};
