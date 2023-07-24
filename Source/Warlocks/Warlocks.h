#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWarlocks, Log, All);

UENUM(BlueprintType)
enum class ESpell : uint8
{
	SpellQ UMETA(DisplayName = "Q Spell"),
	SpellW UMETA(DisplayName = "W Spell"),
	SpellE UMETA(DisplayName = "E Spell"),
	SpellR UMETA(DisplayName = "R Spell"),
};

UENUM(BlueprintType)
enum class EWarlocksAbilityInputID : uint8
{
	// 0 None
	None			UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm			UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel			UMETA(DisplayName = "Cancel"),
	// 3 LMB
	MoveTo			UMETA(DisplayName = "MoveTo"),
	// 5 Q
	AbilityQ		UMETA(DisplayName = "AbilityQ"),
	// 6 W
	AbilityW		UMETA(DisplayName = "AbilityW"),
	// 7 E
	AbilityE		UMETA(DisplayName = "AbilityE"),
	// 8 R
	AbilityR		UMETA(DisplayName = "AbilityR")
};
