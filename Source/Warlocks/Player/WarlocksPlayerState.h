#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "WarlocksPlayerState.generated.h"

UCLASS()
class WARLOCKS_API AWarlocksPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY()
	FString PlayerName = "Placeholder name";

	UFUNCTION()
	virtual void Reset() override;
};
