#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksProjectileSpell.generated.h"

UCLASS(Abstract)
class WARLOCKS_API AWarlocksProjectileSpell : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksProjectileSpell();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UStaticMeshComponent* ProjectileMesh;

	// stats

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float ProjectileSpeed = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	int ProjectileCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	int ProjectileSpread = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stats)
	float ProjectileHitboxRadius = 50;
};
