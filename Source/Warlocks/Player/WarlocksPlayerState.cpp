#include "WarlocksPlayerState.h"

#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Warlocks/Abilities/WarlocksAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "Warlocks/Abilities/WarlocksGameplayAbility.h"

AWarlocksPlayerState::AWarlocksPlayerState()
{
	bReplicates = true;

	// Create ability system component, and set it to be explicitly replicated
	AbilitySystemComponent = CreateDefaultSubobject<UWarlocksAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies.
	// If another GDPlayerState (Hero) receives a GE, we won't be told about it by the Server.
	// Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Create the attribute set, this replicates by default
	// Adding it as a subobject of the owning actor of an AbilitySystemComponent
	// automatically registers the AttributeSet with the AbilitySystemComponent
	AttributeSet = CreateDefaultSubobject<UWarlocksAttributeSet>(TEXT("AttributeSetBase"));

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	NetUpdateFrequency = 10.0f;
}

void AWarlocksPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AbilitySystemComponent))
	{
		AddStartingAbilities();
	}
}

UAbilitySystemComponent* AWarlocksPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UWarlocksAttributeSet* AWarlocksPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AWarlocksPlayerState::Stun()
{
}

UGameplayAbility* AWarlocksPlayerState::GetAbilityInstance(const ESpell SpellSlot) const
{
	switch (SpellSlot)
	{
	case ESpell::SpellQ: return QAbilitySpec.Ability;
	case ESpell::SpellW: return WAbilitySpec.Ability;
	case ESpell::SpellE: return EAbilitySpec.Ability;
	case ESpell::SpellR: return RAbilitySpec.Ability;
	default:
		unimplemented();
		return nullptr;
	}
}

TSubclassOf<UGameplayAbility> AWarlocksPlayerState::GetAbilityClass(const ESpell SpellSlot) const
{
	if (const auto Instance = GetAbilityInstance(SpellSlot))
		return Instance->GetClass();
	return nullptr;
}

FCooldownData AWarlocksPlayerState::GetAbilityCooldownData(const ESpell SpellSlot) const
{
	const auto Ability = Cast<UWarlocksGameplayAbility>(GetAbilityInstance(SpellSlot));
	if (!Ability)
		return {0.f, 1.f};
	
	if (!AbilitySystemComponent || Ability->CooldownTags.IsEmpty())
		return {0.f, Ability->CooldownDuration};
	
	FGameplayEffectQuery const Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(Ability->CooldownTags);
	TArray<float> DurationAndTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemaining(Query);

	float CooldownRemaining = 0.f;
	
	if (DurationAndTimeRemaining.Num() > 0)
	{
		CooldownRemaining = DurationAndTimeRemaining[0];

		for (const float Time : DurationAndTimeRemaining)
		{
			if (Time > CooldownRemaining)
			{
				CooldownRemaining = Time;
			}
		}
	}

	return {CooldownRemaining, Ability->CooldownDuration};
}

FGameplayAbilitySpec
AWarlocksPlayerState::MakeStartingAbilitySpec(const TSubclassOf<UGameplayAbility>& StartupAbility,
                                              EWarlocksAbilityInputID AbilityInputID)
{
	return {StartupAbility, 1, static_cast<int32>(AbilityInputID), this};
}

void AWarlocksPlayerState::AddStartingAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority
		|| !IsValid(AbilitySystemComponent)
		|| AbilitySystemComponent->bStartupAbilitiesGiven)
		return;

	QAbilitySpec = MakeStartingAbilitySpec(QStartupAbility, EWarlocksAbilityInputID::AbilityQ);
	WAbilitySpec = MakeStartingAbilitySpec(WStartupAbility, EWarlocksAbilityInputID::AbilityW);
	EAbilitySpec = MakeStartingAbilitySpec(EStartupAbility, EWarlocksAbilityInputID::AbilityE);
	RAbilitySpec = MakeStartingAbilitySpec(RStartupAbility, EWarlocksAbilityInputID::AbilityR);

	AbilitySystemComponent->GiveAbility(MakeStartingAbilitySpec(MoveToAbility, EWarlocksAbilityInputID::MoveTo));
	AbilitySystemComponent->GiveAbility(QAbilitySpec);
	AbilitySystemComponent->GiveAbility(WAbilitySpec);
	AbilitySystemComponent->GiveAbility(EAbilitySpec);
	AbilitySystemComponent->GiveAbility(RAbilitySpec);

	AbilitySystemComponent->bStartupAbilitiesGiven = true;
}
