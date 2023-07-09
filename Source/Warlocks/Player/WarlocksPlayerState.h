#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Warlocks/Items/WarlocksItem.h"
#include "WarlocksPlayerState.generated.h"

UCLASS(BlueprintType)
class WARLOCKS_API AWarlocksPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsDead = false;

	UPROPERTY(BlueprintReadOnly)
	FString PlayerName = "Placeholder name";

	UPROPERTY(BlueprintReadOnly)
	TArray<FWarlocksItem> Inventory = {
		WarlocksItems[0], WarlocksItems[1], WarlocksItems[2],
		NullItem, NullItem, NullItem
	};

	UFUNCTION()
	virtual void Reset() override;
};
