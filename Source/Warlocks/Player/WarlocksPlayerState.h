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
	AWarlocksPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly)
	TArray<FWarlocksItem> Inventory = {
		WarlocksItems[0], WarlocksItems[1], WarlocksItems[2],
		NullItem, NullItem, NullItem
	};

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsDead = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsStunned = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsCastingSpell = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsChannelingSpell = false;

	// used for playing the winner animation
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsVictorious = false;

	UFUNCTION()
	virtual void Reset() override;
};
