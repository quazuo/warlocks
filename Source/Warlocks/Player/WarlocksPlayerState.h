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

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** The set of items held by the player */
	UPROPERTY(BlueprintReadOnly)
	TArray<FWarlocksItem> Inventory = {
		WarlocksItems[0], WarlocksItems[1], WarlocksItems[2],
		NullItem, NullItem, NullItem
	};

	/** Is the character controlled by this player dead? */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsDead = false;

	/** Is the character controlled by this player stunned (e.g. being knocked back)? */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsStunned = false;

	/** Is the character controlled by this player currently casting a spell? */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsCastingSpell = false;

	/** Is the character controlled by this player currently channelling a spell? */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsChannelingSpell = false;

	/** Is the character controlled by this player the only remaining player? (used for playing the "winner" animation) */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	bool bIsVictorious = false;

	/** Reset the state to original values */
	UFUNCTION()
	virtual void Reset() override;
};
