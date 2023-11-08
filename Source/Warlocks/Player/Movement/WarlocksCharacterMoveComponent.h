#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WarlocksCharacterMoveComponent.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksCharacterMoveComponent final : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UWarlocksCharacterMoveComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
