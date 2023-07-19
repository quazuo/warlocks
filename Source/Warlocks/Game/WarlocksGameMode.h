#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarlocksGameMode.generated.h"

class AStaticMeshActor;

UCLASS(minimalapi)
class AWarlocksGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWarlocksGameMode();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

public:
	// gamemode constants

	/** The damage lava applies per second to characters standing on it */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LavaDamage = 1;

	/** The scale of the safe zone at the beginning of each round */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundBeginSafeZoneScale = 30;

	/** The minimal scale of the safe zone, below which it cannot shrink anymore */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinSafeZoneScale = 15;

	/** The scale difference by which the safe zone shrinks */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SafeZoneShrinkDiff = 2.5;

	/** The time between each shrinks of the safe zone */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int SafeZoneShrinkInterval = 30;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentSafeZoneScale = RoundBeginSafeZoneScale;

	UFUNCTION(BlueprintCallable)
	float GetCurrentSafeZoneRadius() const;

	/**
	 * The time which passes between round end and start. During the transition, the winner's character plays
	 * a special winner animation and the winner's name is announced.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundTransitionTime = 5;

private:
	UPROPERTY()
	AStaticMeshActor* SafeZone;

	UFUNCTION()
	void ResetSafeZone();

	UFUNCTION()
	void ShrinkSafeZone();

	FTimerHandle SafeZoneTimer;
	FTimerHandle RoundTransitionTimer;

	bool bIsRoundTransition = false;

	UFUNCTION()
	void ResetPlayers();

	UFUNCTION()
	void StartRound();

	UFUNCTION()
	void EndRound(AWarlocksPlayerState* WinnerState);
};



