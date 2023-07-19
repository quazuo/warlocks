#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksArcaneBarrier.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksArcaneBarrier final : public AWarlocksSpell
{
	GENERATED_BODY()
	
public:
	AWarlocksArcaneBarrier();

	static TSubclassOf<UObject> GetBPClassPtr();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

private:
	UPROPERTY(VisibleDefaultsOnly, Category = Spell)
	UStaticMeshComponent* Mesh;
};
