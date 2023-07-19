#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarlocksStatusEffect.generated.h"

class UParticleSystemComponent;

UCLASS()
class WARLOCKS_API AWarlocksStatusEffect : public AActor
{
	GENERATED_BODY()

public:
	AWarlocksStatusEffect();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(EditAnywhere, Category = Effect)
	float Duration;
	
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* ContinuousParticle;
};
