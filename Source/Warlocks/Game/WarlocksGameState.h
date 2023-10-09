#pragma once

#include "CoreMinimal.h"
#include "Actors/WarlocksAnnouncer.h"
#include "GameFramework/GameStateBase.h"
#include "WarlocksGameState.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AWarlocksGameState();
	
	virtual void BeginPlay() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastResetSafeZone(AWarlocksSafeZone* SafeZone);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AWarlocksAnnouncer* GetAnnouncer() const { return Announcer; }

private:
	UPROPERTY()
	AWarlocksAnnouncer* Announcer;
};
