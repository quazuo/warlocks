#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WarlocksGameState.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void Announce(const FString &Text);

	UFUNCTION(BlueprintCallable)
	bool IsAnnouncement() const;

	UFUNCTION(BlueprintCallable)
	FString GetAnnouncement();

private:
	TQueue<FString> AnnouncementQueue;
};
