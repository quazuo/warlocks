#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WarlocksCharacter.generated.h"

class AWarlocksStatusEffect;

UCLASS(Blueprintable)
class AWarlocksCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWarlocksCharacter();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	float MaxHealth = 100;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	float Health = MaxHealth;

	/** Array of `WarlockStatusEffect`s currently affecting this character */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	TArray<AWarlocksStatusEffect*> ActiveEffects;

	UFUNCTION()
	void Launch(FVector Direction, const float Force);

	UFUNCTION()
	virtual float TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION()
	float RestoreHealth(const float HealAmount);

	UFUNCTION(Server, Reliable)
	void ServerDie();

	UFUNCTION()
	void ApplyItems();
};
