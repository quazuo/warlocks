#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "Containers/Queue.h"
#include "Warlocks/Player/WarlocksPlayerState.h"

#include "WarlocksGameMode.generated.h"

UCLASS(minimalapi)
class AWarlocksGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AWarlocksGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	// gamemode constants
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LavaTickDamage = 0.05;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundBeginSafeZoneScale = 30;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinSafeZoneScale = 15;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SafeZoneShrinkDiff = 2.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int SafeZoneShrinkInterval = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentSafeZoneScale = RoundBeginSafeZoneScale;

	UFUNCTION(BlueprintCallable)
	float GetCurrentSafeZoneRadius() const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RoundTransitionTime = 5;

	// game flow

	UFUNCTION(BlueprintCallable)
	void StartRound();

	UFUNCTION()
	void EndRound(const AWarlocksPlayerState* Winner);

	// announcements
	
	UFUNCTION(BlueprintCallable)
	void Announce(const FString &Text);

	UFUNCTION(BlueprintCallable)
	bool IsAnnouncement() const;

	UFUNCTION(BlueprintCallable)
	FString GetAnnouncement();

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
	
	TQueue<FString> AnnouncementQueue;
};



