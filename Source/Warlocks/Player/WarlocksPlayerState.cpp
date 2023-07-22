#include "WarlocksPlayerState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Warlocks/Abilities/WarlocksAttributeSet.h"
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

void AWarlocksPlayerState::Stun() {

}

FGameplayAbilitySpec
AWarlocksPlayerState::GetStartingAbilitySpec(const TSubclassOf<UWarlocksGameplayAbility>& StartupAbility,
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

	AbilitySystemComponent->GiveAbility(GetStartingAbilitySpec(MoveToAbility, EWarlocksAbilityInputID::MoveTo));
	AbilitySystemComponent->GiveAbility(GetStartingAbilitySpec(QStartupAbility, EWarlocksAbilityInputID::AbilityQ));
	AbilitySystemComponent->GiveAbility(GetStartingAbilitySpec(WStartupAbility, EWarlocksAbilityInputID::AbilityW));
	AbilitySystemComponent->GiveAbility(GetStartingAbilitySpec(EStartupAbility, EWarlocksAbilityInputID::AbilityE));
	AbilitySystemComponent->GiveAbility(GetStartingAbilitySpec(RStartupAbility, EWarlocksAbilityInputID::AbilityR));
	
	AbilitySystemComponent->bStartupAbilitiesGiven = true;
}
