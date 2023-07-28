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
	
	/**
	 * The time which passes between round end and start. During the transition, the winner's character plays
	 * a special winner animation and the winner's name is announced.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RoundTransitionTime = 5.f;

private:
	UPROPERTY(VisibleAnywhere)
	class AWarlocksSafeZone* SafeZone;
	
	FTimerHandle RoundTransitionTimer;

	bool bIsRoundTransition = false;

	UFUNCTION()
	void ResetPlayers();

	UFUNCTION()
	void StartRound();

	UFUNCTION()
	void EndRound(const AWarlocksPlayerState* WinnerState);
};



