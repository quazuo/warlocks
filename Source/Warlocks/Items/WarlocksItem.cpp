#include "WarlocksItem.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Effects/Debuffs/WarlocksDotEffect.h"

#include "Warlocks/Spells/Projectile/WarlocksFireball.h"
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
	//Character->MaxHealth /= 2;
	//Character->Health = Character->MaxHealth;
}

void FireKeeperCharm_SpellOnHitEffect(AWarlocksCharacter* Target)
{
	static const auto DotClassObjPath =
		TEXT("/Script/Engine.Blueprint'/Game/Warlocks/Blueprints/Items/FireKeeperCharm/BP_FireKeeperCharmDotEffect.BP_FireKeeperCharmDotEffect'");
	static const TSubclassOf<UObject> DotClass = FWarlocksUtils::GetBPClassPtr_Runtime(DotClassObjPath);
	if (!DotClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Target;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Target->GetWorld()->SpawnActor<AWarlocksDotEffect>(DotClass, Target->GetActorLocation(), Target->GetActorRotation(),
	                                                  SpawnParams);
}

// Generic modifications

FSpellModifier MakePowerModifier(const float Mod)
{
	return [Mod](AWarlocksSpell* Spell) { Spell->Power *= Mod; };
}

FSpellModifier MakeKnockbackModifier(const float Mod)
{
	return [Mod](AWarlocksSpell* Spell) { Spell->Knockback *= Mod; };
}

FCharacterModifier MakeMaxHealthModifier(const float Mod)
{
	return [Mod](AWarlocksCharacter* Char) { /* Char->MaxHealth += Mod; */ };
}
