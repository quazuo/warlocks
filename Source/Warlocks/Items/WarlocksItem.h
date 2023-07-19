#pragma once

#include "CoreMinimal.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "WarlocksItem.generated.h"

class AWarlocksPlayerState;
class AWarlocksSpell;
class AWarlocksCharacter;

using FSpellModifier = std::function<void(AWarlocksSpell*)>;
using FSpellOnHitEffect = std::function<void(AWarlocksCharacter*)>;
using FCharacterModifier = std::function<void(AWarlocksCharacter*)>;

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
	
	FSpellModifier SpellModifier = nullptr;

	FSpellOnHitEffect SpellOnHitEffect = nullptr;

	FCharacterModifier CharacterModifier = nullptr;
};

// Spell modifiers

FSpellModifier MakePowerModifier(const float Mod);
FSpellModifier MakeKnockbackModifier(const float Mod);

#define SPELLMOD(name) void name##_SpellMod(AWarlocksSpell*)
SPELLMOD(InfernalFork);
SPELLMOD(BrokenHourglass);
SPELLMOD(GlassCannon);

// Spell on-hit modifiers

#define SPELLONHITMOD(name) void name##_SpellOnHitEffect(AWarlocksCharacter*)
SPELLONHITMOD(FireKeeperCharm);

// Character modifiers

#define CHARMOD(name) void name##_CharMod(AWarlocksCharacter*)
CHARMOD(GlassCannon);

// Item table

static const TArray<FWarlocksItem> WarlocksItems = {
	// Common items
	{
		"LongSword",
		EItemRarity::Common,
		MakePowerModifier(1.1)
	},
	// Uncommon items
	{
		"FireKeeperCharm",
		EItemRarity::Uncommon,
		nullptr,
		FireKeeperCharm_SpellOnHitEffect
	},
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
	{
		"BloodOrb",
		EItemRarity::Rare,
		// todo ideas
	},
	// Epic items
	{
		"GlassCannon",
		EItemRarity::Epic,
		GlassCannon_SpellMod,
		nullptr,
		GlassCannon_CharMod
	},
	{
		"ForbiddenTome",
		EItemRarity::Epic,
		// todo ideas
	},
	{
		"AccursedEye",
		EItemRarity::Epic,
		// todo ideas
	}
	// Legendary items
	
};

static const FWarlocksItem NullItem = { "Empty" };
