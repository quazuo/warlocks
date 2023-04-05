// Copyright Epic Games, Inc. All Rights Reserved.

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

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void Launch(FVector Direction, const float Force);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsCastingSpell = false;
	FORCEINLINE void StartCastingSpell() { bIsCastingSpell = true; }
	FORCEINLINE void StopCastingSpell() { bIsCastingSpell = false; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	bool bIsChannelingSpell = false;
	FORCEINLINE void StartChannelingSpell() { bIsChannelingSpell = true; }
	FORCEINLINE void StopChannelingSpell() { bIsChannelingSpell = false; }

	UPROPERTY(EditAnywhere)
	FColor PlayerColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Health = MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDead = false;

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	float RestoreHealth(const float HealAmount);

	UFUNCTION(BlueprintCallable)
	void Die();
};
