#pragma once

#include "Warlocks/Abilities/Spells/WarlocksSpellActor.h"
#include "WarlocksMeditateAura.generated.h"

class UParticleSystemComponent;
class UPointLightComponent;

UCLASS()
class WARLOCKS_API AWarlocksMeditateAura final : public AWarlocksSpellActor
{
	GENERATED_BODY()

public:
	AWarlocksMeditateAura();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = Projectile)
	UParticleSystemComponent* ContinuousParticle;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UPointLightComponent* PointLight;
};
