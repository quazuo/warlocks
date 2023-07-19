#include "WarlocksDotEffect.h"

#include "Kismet/GameplayStatics.h"
#include "Warlocks/Player/WarlocksCharacter.h"

void AWarlocksDotEffect::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const auto OwnerWarlock = Cast<AWarlocksCharacter>(GetOwner());
	if (!OwnerWarlock) return;
	
	const auto OwnerController = OwnerWarlock->GetController();
	if (!OwnerController) return;
	
	UGameplayStatics::ApplyDamage(OwnerWarlock, Power * DeltaSeconds, OwnerController, this, nullptr);
}
