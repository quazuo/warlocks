

#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksFlash.generated.h"

/**
 * 
 */
UCLASS()
class WARLOCKS_API AWarlocksFlash : public AWarlocksSpell
{
	GENERATED_BODY()

public:
	AWarlocksFlash();

	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particle)
	UParticleSystem* OnCastParticle;
};
