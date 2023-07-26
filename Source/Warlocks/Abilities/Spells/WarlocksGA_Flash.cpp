#include "WarlocksGA_Flash.h"

#include "Kismet/GameplayStatics.h"
#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UWarlocksGA_Flash::UWarlocksGA_Flash()
{
	SpellName = "Flash";
	CastTime = 0.f;
	CooldownDuration = 15.f;

	const FGameplayTag FlashTag = FGameplayTag::RequestGameplayTag("Ability.Spell.Flash");
	AbilityTags.AddTag(FlashTag);

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	ActivationBlockedTags.AddTag(StunTag);
	
	const FGameplayTag SpellTag = FGameplayTag::RequestGameplayTag("Ability.Spell");
	CancelAbilitiesWithTag.AddTag(SpellTag);
}

void UWarlocksGA_Flash::ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
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

void UWarlocksGA_Flash::OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnSpellCastFinish(EventTag, EventData);

	if (const auto Warlock = Cast<AWarlocksCharacter>(GetAvatarActorFromActorInfo()))
	{
		// spawn initial particle
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
		                                         FRotator::ZeroRotator, FVector(.25));

		if (Warlock->GetController())
			Warlock->GetController()->StopMovement();
		Warlock->GetCharacterMovement()->Velocity = FVector::Zero();

		FVector FlashDirection = CachedTarget - Warlock->GetActorLocation();
		FlashDirection.Z = 0;
		FlashDirection.Normalize();
		
		Warlock->SetActorLocation(Warlock->GetActorLocation() + FlashDirection * Range);

		// spawn next particle
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnCastParticle, Warlock->GetActorLocation(),
		                                         FRotator::ZeroRotator, FVector(.25));
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
