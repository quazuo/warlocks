

#pragma once

#include "CoreMinimal.h"
#include "WarlocksSpell.h"
#include "WarlocksArcaneBarrier.generated.h"

/**
 * 
 */
UCLASS()
class WARLOCKS_API AWarlocksArcaneBarrier final : public AWarlocksSpell
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
