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
	void Launch(FVector Direction);

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
	float Health = 10; // todo - change to MaxHealth

	UFUNCTION(BlueprintCallable)
	void ModifyHealth(float Value);
};
