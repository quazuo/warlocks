#pragma once

#include "CoreMinimal.h"
#include "WarlocksItem.generated.h"

class AWarlocksPlayerState;
class AWarlocksSpell;
class AWarlocksCharacter;

// todo: kolejnosc itemkow

UENUM()
enum class EItemRarity { Common, Uncommon, Rare, Epic, Legendary };

USTRUCT(BlueprintType)
struct WARLOCKS_API FWarlocksItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Name;
	
	UPROPERTY(BlueprintReadOnly)
	EItemRarity Rarity;
	
	std::function<void(AWarlocksSpell*)> SpellModifier = nullptr;

	std::function<void(AWarlocksCharacter*)> CharacterModifier = nullptr;
};

// Spell modifiers
#define SPELLMOD(name) void name##_SpellMod(AWarlocksSpell*)
SPELLMOD(InfernalFork);
SPELLMOD(BrokenHourglass);
SPELLMOD(GlassCannon);

// Character modifiers
#define CHARMOD(name) void name##_CharMod(AWarlocksCharacter*)
CHARMOD(GlassCannon);

static const FWarlocksItem NullItem = { "None" };

static const TArray<FWarlocksItem> WarlocksItems = {
	// Common items
	
	// Uncommon items
	
	// Rare items
	{
		"InfernalFork",
		EItemRarity::Rare,
		InfernalFork_SpellMod
	},
	{
		"BrokenHourglass",
		EItemRarity::Rare,
		BrokenHourglass_SpellMod
	},
	// Epic items
	{
		"GlassCannon",
		EItemRarity::Epic,
		GlassCannon_SpellMod,
		GlassCannon_CharMod
	}
	// Legendary items
	
};
