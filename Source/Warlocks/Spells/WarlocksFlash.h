#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksFlash.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksFlash final : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksFlash();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = Particle)
	UParticleSystem* OnCastParticle;
};
