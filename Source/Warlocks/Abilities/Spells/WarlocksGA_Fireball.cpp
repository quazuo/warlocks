#include "WarlocksGA_Fireball.h"

#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/Tasks/WarlocksAT_CastSpell.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Spells/Projectile/WarlocksFireball.h"

UWarlocksGA_Fireball::UWarlocksGA_Fireball()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FireballClass = FWarlocksUtils::GetBPClassPtr(
		TEXT("/Script/Engine.Blueprint'/Game/Warlocks/Blueprints/Spells/BP_WarlocksFireball.BP_WarlocksFireball'"));

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	ActivationBlockedTags.AddTag(StunTag);
}

void UWarlocksGA_Fireball::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const TWeakObjectPtr<APlayerController> Controller = ActorInfo ? ActorInfo->PlayerController : nullptr;
	if (!Controller.IsValid())
		return;

	FHitResult Hit;
	if (!Controller->GetHitResultUnderCursor(ECC_Visibility, true, Hit))
		return;

	CachedTarget = Hit.Location;

	UWarlocksAT_CastSpell* Task = UWarlocksAT_CastSpell::TaskCastSpell(this, CastTime);
	Task->OnFinish.AddDynamic(this, &UWarlocksGA_Fireball::OnCompleted);
	Task->ReadyForActivation();
}

bool UWarlocksGA_Fireball::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo,
                                              const FGameplayTagContainer* SourceTags,
                                              const FGameplayTagContainer* TargetTags,
                                              FGameplayTagContainer* OptionalRelevantTags) const
{
	// todo - cooldown
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UWarlocksGA_Fireball::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UWarlocksGA_Fireball::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
	{
		const auto Warlock = Cast<AWarlocksCharacter>(GetAvatarActorFromActorInfo());
		if (!Warlock)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		const FVector FireballLocation = Warlock->GetActorLocation();
		const FRotator FireballRotation = (CachedTarget - FireballLocation).Rotation();
		const FTransform FireballTransform(FireballRotation, FireballLocation);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		auto Fireball = GetWorld()->SpawnActorDeferred<AWarlocksFireball>(
			FireballClass,
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

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
