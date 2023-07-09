#include "WarlocksItem.h"

#include "Warlocks/Spells/WarlocksFireball.h"
#include "Kismet/KismetMathLibrary.h"
#include "Warlocks/Player/WarlocksCharacter.h"

void InfernalFork_SpellMod(AWarlocksSpell* Spell)
{
	const auto Fireball = Cast<AWarlocksFireball>(Spell);
	if (Fireball)
	{
		Fireball->ProjectileCount += 2;
	}
}

void BrokenHourglass_SpellMod(AWarlocksSpell* Spell)
{
	static constexpr float Chance = 0.25;
	
	if (UKismetMathLibrary::RandomFloat() <= Chance)
	{
		Spell->CastTime = 0;
	}
}

void GlassCannon_SpellMod(AWarlocksSpell* Spell)
{
	Spell->Power *= 2;
}

void GlassCannon_CharMod(AWarlocksCharacter* Character)
{
	Character->MaxHealth /= 2;
	Character->Health = Character->MaxHealth;
}
