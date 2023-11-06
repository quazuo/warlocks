#include "WarlocksAbilitySystemComponent.h"

#include "Warlocks/Player/WarlocksPlayerController.h"
#include "Warlocks/Player/WarlocksPlayerState.h"

void UWarlocksAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);
	
	if (const auto PlayerState = Cast<AWarlocksPlayerState>(GetOwnerActor()))
	{
		if (const auto Controller = Cast<AWarlocksPlayerController>(PlayerState->GetPlayerController()))
		{
			Controller->UpdateAbilityUI();
		}
	}
}
