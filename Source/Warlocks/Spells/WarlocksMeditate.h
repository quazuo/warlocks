#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksMeditate.generated.h"

class UParticleSystemComponent;
class UPointLightComponent;

UCLASS()
class WARLOCKS_API AWarlocksMeditate final : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksMeditate();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UPointLightComponent* PointLight;
};
