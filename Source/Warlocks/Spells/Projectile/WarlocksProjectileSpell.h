#pragma once

#include "CoreMinimal.h"
#include "Warlocks/Spells/WarlocksSpell.h"
#include "WarlocksProjectileSpell.generated.h"

UCLASS(Abstract)
class WARLOCKS_API AWarlocksProjectileSpell : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksProjectileSpell();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
					   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
public:
	UFUNCTION()
	virtual void SpawnOnHitParticle() {}
	
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
};
