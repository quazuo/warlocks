#pragma once

#include "CoreMinimal.h"
#include "Warlocks/Effects/WarlocksStatusEffect.h"
#include "WarlocksDotEffect.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksDotEffect final : public AWarlocksStatusEffect
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaSeconds) override;

private:
	/** The power of this status effect, which mostly means the damage or healing it applies per second */
	UPROPERTY(EditDefaultsOnly, Category = Effect)
	float Power;
};
