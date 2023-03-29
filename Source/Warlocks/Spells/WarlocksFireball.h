// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "WarlocksFireball.generated.h"

/**
 * 
 */
UCLASS()
class WARLOCKS_API AWarlocksFireball : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksFireball();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Movement)
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Projectile)
	UPointLightComponent* PointLight;

private:
	UPROPERTY(EditAnywhere)
	UParticleSystem* OnHitParticle;
};
