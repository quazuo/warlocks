#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WarlocksCharacter.generated.h"

UCLASS(Blueprintable)
class AWarlocksCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWarlocksCharacter();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void Launch(FVector Direction, const float Force);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsCastingSpell = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsChannelingSpell = false;

	// used for playing the winner animation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsVictorious = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = State)
	float Health = MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	float RestoreHealth(const float HealAmount);

	UFUNCTION(BlueprintCallable)
	void Die();
};
