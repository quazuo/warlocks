// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksArcaneBarrier.generated.h"

/**
 * 
 */
UCLASS()
class WARLOCKS_API AWarlocksArcaneBarrier : public AWarlocksSpell
{
	GENERATED_BODY()

	AWarlocksArcaneBarrier();

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

	UPROPERTY(VisibleDefaultsOnly, Category = Spell)
	class USphereComponent* CollisionSphere;

	UPROPERTY(VisibleDefaultsOnly, Category = Spell)
	UStaticMeshComponent* Mesh;
};
