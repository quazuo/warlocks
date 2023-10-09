#include "WarlocksPlayerState.h"

#include "Warlocks/Warlocks.h"
#include "Warlocks/Abilities/WarlocksAbilitySystemComponent.h"
#include "Warlocks/Abilities/WarlocksAttributeSet.h"
#include "AbilitySystemGlobals.h"
#include "AI/NavigationSystemBase.h"
#include "Kismet/GameplayStatics.h"
#include "Warlocks/Abilities/WarlocksGameplayAbility.h"
#include "Warlocks/Game/WarlocksGameState.h"

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

	// Cache tags
	StunCancelTags.AddTag(FGameplayTag::RequestGameplayTag("Ability"));
	StunTag = FGameplayTag::RequestGameplayTag("Player.State.Stun");
	DeadTag = FGameplayTag::RequestGameplayTag("Player.State.Dead");

	PrimaryActorTick.bCanEverTick = true;
}

void AWarlocksPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(AbilitySystemComponent))
	{
		AddStartingAbilities();

		if (HealthRegenGE)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(HealthRegenGE.GetDefaultObject(), 1, {});
		}
	}
}

void AWarlocksPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AbilitySystemComponent && IsDead() && !AbilitySystemComponent->HasMatchingGameplayTag(DeadTag)) // has just died
	{
		OnDeath();
	}
}

void AWarlocksPlayerState::Reset()
{
	Super::Reset();
	AttributeSet->Reset();
	AbilitySystemComponent->RemoveActiveEffects({});
}

UAbilitySystemComponent* AWarlocksPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UWarlocksAttributeSet* AWarlocksPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

bool AWarlocksPlayerState::IsDead() const
{
	if (!AttributeSet)
	{
		UE_LOG(LogWarlocks, Error, TEXT("Tried to call IsDead() without a valid AttributeSet"));
		return false;
	}

	return AttributeSet->GetHealth() == 0;
}

void AWarlocksPlayerState::ApplyDamage(const float Damage)
{
	if (!AbilitySystemComponent) return;

	if (!DamageGE)
	{
		UE_LOG(LogWarlocks, Warning, TEXT("Tried to apply damage without DamageGE set to a valid GE"));
		return;
	}

	const FGameplayEffectSpecHandle DamageEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGE, 1, {});
	if (const FGameplayEffectSpec* EffectSpec = DamageEffectSpecHandle.Data.Get())
	{
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag("Data.Damage"),
			Damage
		);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpec);
	}
}

void AWarlocksPlayerState::StartCheering()
{
	const FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag("Ability");
	const FGameplayTagContainer AbilityTags(AbilityTag);
	AbilitySystemComponent->CancelAbilities(&AbilityTags);

	if (!CheerGE)
	{
		UE_LOG(LogWarlocks, Warning, TEXT("Tried to start cheering without CheerGE set to a valid GE"));
		return;
	}
	AbilitySystemComponent->ApplyGameplayEffectToSelf(CheerGE.GetDefaultObject(), 1, {});
}

void AWarlocksPlayerState::ApplyStun()
{
	AbilitySystemComponent->CancelAbilities(&StunCancelTags);

	if (!StunGE)
	{
		UE_LOG(LogWarlocks, Warning, TEXT("Tried to apply stun without StunGE set to a valid GE"));
		return;
	}
	AbilitySystemComponent->ApplyGameplayEffectToSelf(StunGE.GetDefaultObject(), 1, {});
}

void AWarlocksPlayerState::RemoveStun()
{
	const FGameplayTagContainer GrantedTags{StunTag};
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(GrantedTags);
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
	// Grant abilities, but only on the server, and as long as they haven't been granted yet
	if (GetLocalRole() != ROLE_Authority
		|| !IsValid(AbilitySystemComponent)
		|| AbilitySystemComponent->bStartupAbilitiesGiven)
		return;

	AbilitySystemComponent->GiveAbility(MakeStartingAbilitySpec(MoveToAbility, EWarlocksAbilityInputID::MoveTo));

	if (QStartupAbility)
	{
		QAbilitySpec = MakeStartingAbilitySpec(QStartupAbility, EWarlocksAbilityInputID::AbilityQ);
		AbilitySystemComponent->GiveAbility(QAbilitySpec);
	}
	if (WStartupAbility)
	{
		WAbilitySpec = MakeStartingAbilitySpec(WStartupAbility, EWarlocksAbilityInputID::AbilityW);
		AbilitySystemComponent->GiveAbility(WAbilitySpec);
	}
	if (EStartupAbility)
	{
		EAbilitySpec = MakeStartingAbilitySpec(EStartupAbility, EWarlocksAbilityInputID::AbilityE);
		AbilitySystemComponent->GiveAbility(EAbilitySpec);
	}
	if (RStartupAbility)
	{
		RAbilitySpec = MakeStartingAbilitySpec(RStartupAbility, EWarlocksAbilityInputID::AbilityR);
		AbilitySystemComponent->GiveAbility(RAbilitySpec);
	}

	AbilitySystemComponent->bStartupAbilitiesGiven = true;
}

void AWarlocksPlayerState::OnDeath()
{
	AbilitySystemComponent->ApplyGameplayEffectToSelf(DeathGE.GetDefaultObject(), 1, {});

	if (const auto Controller = GetPlayerController())
		Controller->StopMovement();

	if (const auto State = Cast<AWarlocksGameState>(UGameplayStatics::GetGameState(GetWorld())))
	{
		for (const auto &Player : State->PlayerArray)
		{
			if (const auto Warlock = Cast<AWarlocksPlayerState>(Player); !Warlock->IsDead())
			{
				Warlock->SetScore(Warlock->GetScore() + 1);
			}
		}	
	}
}
