#include "WarlocksGA_Meditate.h"

#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerState.h"
#include "WarlocksMeditateAura.h"

UWarlocksGA_Meditate::UWarlocksGA_Meditate()
{
	SpellName = "Meditate";
	CastTime = 0.5;
	ChannelTime = 5.f;
	CooldownDuration = 20.f;

	const FGameplayTag MeditateTag = FGameplayTag::RequestGameplayTag("Ability.Spell.Meditate");
	AbilityTags.AddTag(MeditateTag);

	const FGameplayTag StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	ActivationBlockedTags.AddTag(StunTag);
}

void UWarlocksGA_Meditate::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (CommitCheck(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
	{
		StartSpellCast(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	}
	else 
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
	}
}

void UWarlocksGA_Meditate::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (MeditateAura)
	{
		MeditateAura->Destroy();
	}
	RemoveGameplayEffectFromOwner(FGameplayTag::RequestGameplayTag("Status.Meditate"));
}

void UWarlocksGA_Meditate::OnSpellCastFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnSpellCastFinish(EventTag, EventData);
	
	ApplySetByCallerEffectToOwner(MeditateRegenGE, FGameplayTag::RequestGameplayTag("Data.Meditate.Regen"), Power);

	if (CurrentActorInfo->IsNetAuthority())
	{
		const auto Warlock = Cast<AWarlocksCharacter>(GetAvatarActorFromActorInfo());
		if (!Warlock)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			return;
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwningActorFromActorInfo();
		SpawnParameters.Instigator = Warlock;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		MeditateAura = GetWorld()->SpawnActor<AWarlocksMeditateAura>(
			MeditateAuraClass,
			Warlock->GetActorTransform(),
			SpawnParameters
		);
	}
}

void UWarlocksGA_Meditate::OnChannelFinish(FGameplayTag EventTag, FGameplayEventData EventData)
{
	Super::OnChannelFinish(EventTag, EventData);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
