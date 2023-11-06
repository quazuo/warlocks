#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "Warlocks/Warlocks.h"
#include "WarlocksPlayerState.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UWarlocksGameplayAbility;
class UWarlocksAttributeSet;
class UWarlocksAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FCooldownData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Remaining;

	UPROPERTY(BlueprintReadOnly)
	float Total;
};

UCLASS(BlueprintType)
class WARLOCKS_API AWarlocksPlayerState final : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWarlocksPlayerState();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Reset() override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~IAbilitySystemInterface

	UFUNCTION(BlueprintCallable)
	UWarlocksAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(const float Damage);

	UFUNCTION(BlueprintCallable)
	void StartCheering();

	UFUNCTION(BlueprintCallable)
	void ApplyStun();

	UFUNCTION(BlueprintCallable)
	void RemoveStun();

	UFUNCTION(BlueprintCallable)
	UGameplayAbility* GetAbilityInstance(const ESpell SpellSlot) const;

	UFUNCTION(BlueprintCallable)
	TSubclassOf<UGameplayAbility> GetAbilityClass(const ESpell SpellSlot) const;

	UFUNCTION(BlueprintCallable)
	FCooldownData GetAbilityCooldownData(const ESpell SpellSlot) const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	UWarlocksAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
	UWarlocksAttributeSet* AttributeSet;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> MoveToAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> QStartupAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> WStartupAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> EStartupAbility;

	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UGameplayAbility> RStartupAbility;

	UPROPERTY(ReplicatedUsing = OnRep_AbilitySpec)
	FGameplayAbilitySpec QAbilitySpec;

	UPROPERTY(ReplicatedUsing = OnRep_AbilitySpec)
	FGameplayAbilitySpec WAbilitySpec;

	UPROPERTY(ReplicatedUsing = OnRep_AbilitySpec)
	FGameplayAbilitySpec EAbilitySpec;

	UPROPERTY(ReplicatedUsing = OnRep_AbilitySpec)
	FGameplayAbilitySpec RAbilitySpec;

	UFUNCTION()
	void OnRep_AbilitySpec() const;

	FGameplayAbilitySpec MakeStartingAbilitySpec(const TSubclassOf<UGameplayAbility>& StartupAbility,
	                                             EWarlocksAbilityInputID AbilityInputID);

	void AddStartingAbilities();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DamageGE;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> HealthRegenGE;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StunGE;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CheerGE;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> DeathGE;

	FGameplayTag StunTag, DeadTag;

	FGameplayTagContainer StunCancelTags;

	void OnDeath();
};
