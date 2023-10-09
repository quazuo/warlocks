#pragma once

#include "WarlocksSafeZone.generated.h"

UCLASS()
class AWarlocksSafeZone : public AActor
{
	GENERATED_BODY()

public:
	AWarlocksSafeZone();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	float GetCurrentSafeZoneRadius() const;

	UFUNCTION(BlueprintCallable)
	void ResetSafeZone();

	UFUNCTION(BlueprintCallable)
	void StopSafeZoneShrinking();

protected:
	UFUNCTION(BlueprintCallable)
	void ShrinkSafeZone();
	
	/** The scale of the safe zone at the beginning of each round */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RoundBeginSafeZoneScale = 30;

	/** The minimal scale of the safe zone, below which it cannot shrink anymore */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinSafeZoneScale = 15;

	/** The scale difference by which the safe zone shrinks */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SafeZoneShrinkDiff = 2.5;

	/** The time between shrinks of the safe zone */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int SafeZoneShrinkInterval = 30;
	
	UPROPERTY(ReplicatedUsing = RepNotify_CurrentSafeZoneScale, VisibleAnywhere, BlueprintReadOnly)
	float CurrentSafeZoneScale = RoundBeginSafeZoneScale;

private:
	UPROPERTY(EditAnywhere, Category = Projectile)
	class UCapsuleComponent* TriggerCapsule;

	UPROPERTY(EditAnywhere, Category = Projectile)
	UStaticMeshComponent* Mesh;

	FTimerHandle ShrinkTimer;

	void UpdateMeshScale(const float Scale);

	void UpdateCapsuleSize() const;

	UFUNCTION()
	void RepNotify_CurrentSafeZoneScale() const;

	void AnnounceSafeZoneShrink() const;
};
