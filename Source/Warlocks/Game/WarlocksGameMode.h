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
	
	// gamemode constants

	/** The damage lava applies per second to characters standing on it */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LavaDamage = 1;

	/**
	 * The time which passes between round end and start. During the transition, the winner's character plays
	 * a special winner animation and the winner's name is announced.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RoundTransitionTime = 5;

private:
	UPROPERTY()
	class AWarlocksSafeZone* SafeZone;

	FTimerHandle SafeZoneTimer;
	FTimerHandle RoundTransitionTimer;

	bool bIsRoundTransition = false;
	
	void TickLavaDamage(const float DeltaTime) const;

	UFUNCTION()
	void ResetPlayers();

	UFUNCTION()
	void StartRound();

	UFUNCTION()
	void EndRound(AWarlocksPlayerState* WinnerState);
};



