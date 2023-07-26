#include "WarlocksGA_Fireball.h"

#include "Warlocks/Abilities/Tasks/WarlocksAT_Delay.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Warlocks/Spells/Projectile/WarlocksProjectileSpell.h"

UWarlocksGA_Fireball::UWarlocksGA_Fireball()
{
	SpellName = "Fireball";
	CastTime = 1.f;
	CooldownDuration = 3.f;

	const FGameplayTag SpellTag = FGameplayTag::RequestGameplayTag("Ability.Spell.Fireball");
	AbilityTags.AddTag(SpellTag);

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	ActivationBlockedTags.AddTag(StunTag);
}

void UWarlocksGA_Fireball::ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                         FGameplayTag ApplicationTag)
{
	Super::ActivateAbilityWithTargetData(TargetDataHandle, ApplicationTag);
	
	// rotate character towards the target
	if (const auto Controller = Cast<AWarlocksPlayerController>(GetCurrentActorInfo()->PlayerController))
	{
		const FVector WarlockLocation = GetCurrentActorInfo()->AvatarActor->GetActorLocation();
		FVector RotationVec = CachedTarget - WarlockLocation;
		RotationVec.Z = 0;
		Controller->RotateCharacter(RotationVec.Rotation());
	}
	
	StartSpellCast(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, &CurrentEventData);
}

void UWarlocksGA_Fireball::OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnSpellCastFinish(EventTag, EventData);

	if (CurrentActorInfo->IsNetAuthority())
	{
		const auto Warlock = Cast<AWarlocksCharacter>(GetAvatarActorFromActorInfo());
		if (!Warlock)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		const FVector FireballLocation = Warlock->GetActorLocation();

		FVector FireballRotationVec = CachedTarget - FireballLocation;
		FireballRotationVec.Z = 0;
		const FRotator FireballRotation = FireballRotationVec.Rotation();

		const FTransform FireballTransform(FireballRotation, FireballLocation);

		auto Fireball = GetWorld()->SpawnActorDeferred<AWarlocksProjectileSpell>(
			ProjectileClass,
			FireballTransform,
			GetOwningActorFromActorInfo(),
			Warlock,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		if (Fireball)
		{
			// Fireball->DamageEffectSpecHandle = DamageEffectSpecHandle;
			Fireball->FinishSpawning(FireballTransform);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
