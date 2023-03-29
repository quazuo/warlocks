// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "Components/PointLightComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "WarlocksMeditate.generated.h"

/**
 * 
 */
UCLASS()
class WARLOCKS_API AWarlocksMeditate : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksMeditate();

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Projectile)
	UPointLightComponent* PointLight;

private:
	const float HealPerTick = 0.1;
};
