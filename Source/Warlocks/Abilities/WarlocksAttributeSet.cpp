#include "WarlocksAttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "Warlocks/Player/WarlocksCharacter.h"
#include "Warlocks/Player/WarlocksPlayerController.h"

UWarlocksAttributeSet::UWarlocksAttributeSet()
{
	InitMaxHealth(100.f);
	InitHealth(GetMaxHealth());
	InitHealthRegen(0.f);
	InitMoveSpeed(100.f);

	// maybe cache tags?
}

void UWarlocksAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UWarlocksAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	const UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AWarlocksCharacter* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AWarlocksCharacter>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AWarlocksCharacter* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (!SourceController && SourceActor)
		{
			if (const auto Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}
		
		SourceCharacter = Cast<AWarlocksCharacter>(SourceController ? SourceController->GetPawn() : SourceActor);

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0)
		{
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = GetHealth() > 0.f;
			}

			// Apply the health change and then clamp it
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			if (TargetCharacter && WasAlive)
			{
				 UE_LOG(LogTemp, Log, TEXT("%s Damage Received: %f"),
				        *GetOwningActor()->GetName(), LocalDamageDone);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
}

void UWarlocksAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UWarlocksAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWarlocksAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWarlocksAttributeSet, HealthRegen, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWarlocksAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UWarlocksAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWarlocksAttributeSet, Health, OldHealth);
}

void UWarlocksAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWarlocksAttributeSet, MaxHealth, OldMaxHealth);
}

void UWarlocksAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWarlocksAttributeSet, HealthRegen, OldHealthRegen);
}

void UWarlocksAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWarlocksAttributeSet, MoveSpeed, OldMoveSpeed);
}
