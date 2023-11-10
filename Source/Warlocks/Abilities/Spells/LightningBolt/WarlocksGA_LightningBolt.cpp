#include "WarlocksGA_LightningBolt.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

UWarlocksGA_LightningBolt::UWarlocksGA_LightningBolt()
{
	SpellName = "LightningBolt";
	CastTime = 0.5f;
	CooldownDuration = 5.f;

	const FGameplayTag SpellTag = FGameplayTag::RequestGameplayTag("Ability.Spell.LightningBolt");
	AbilityTags.AddTag(SpellTag);
}

void UWarlocksGA_LightningBolt::ActivateAbilityWithTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle,
                                                              FGameplayTag ApplicationTag)
{
	Super::ActivateAbilityWithTargetData(TargetDataHandle, ApplicationTag);

	// rotate character towards the target
	const auto Controller = Cast<AWarlocksPlayerController>(GetCurrentActorInfo()->PlayerController);
	if (Controller && CachedTarget)
	{
		const FVector WarlockLocation = GetCurrentActorInfo()->AvatarActor->GetActorLocation();
		FVector RotationVec = CachedTarget->GetActorLocation() - WarlockLocation;
		RotationVec.Z = 0;
		Controller->RotateCharacter(RotationVec.Rotation());
	}

	StartSpellCast(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, &CurrentEventData);
}

void UWarlocksGA_LightningBolt::OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnSpellCastFinish(EventTag, EventData);

	const auto Warlock = Cast<AWarlocksCharacter>(GetAvatarActorFromActorInfo());
	if (!Warlock || !CachedTarget)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	if (OnCastParticle)
	{
		const FVector CasterLocation = Warlock->GetActorLocation();
		const FVector TargetLocation = CachedTarget->GetActorLocation();

		UNiagaraComponent* Particle =
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), OnCastParticle, CasterLocation);
		Particle->SetVectorParameter("BeamEnd", TargetLocation - CasterLocation); // relative location
	}

	if (CurrentActorInfo->IsNetAuthority())
	{
		// todo - actually apply ability
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}