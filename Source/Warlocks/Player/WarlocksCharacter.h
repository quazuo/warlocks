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
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void BeginPlay() override;
	
	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	float MaxHealth = 100;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = State)
	float Health = MaxHealth;

	UFUNCTION()
	void Launch(FVector Direction, const float Force);

	UFUNCTION(BlueprintCallable)
	virtual float TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	float RestoreHealth(const float HealAmount);

	UFUNCTION(Server, Reliable)
	void ServerDie();

	UFUNCTION(BlueprintCallable)
	void ApplyItems();
};
