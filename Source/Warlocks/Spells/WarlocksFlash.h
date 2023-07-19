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

	static TSubclassOf<UObject> GetBPClassPtr();

protected:
	virtual void BeginPlay() override;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category = Particle)
	UParticleSystem* OnCastParticle;
};
