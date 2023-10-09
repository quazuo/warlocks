#include "WarlocksGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Actors/WarlocksSafeZone.h"
#include "Warlocks/FWarlocksUtils.h"
#include "Warlocks/Warlocks.h"

AWarlocksGameState::AWarlocksGameState()
{
}

void AWarlocksGameState::BeginPlay()
{
	Super::BeginPlay();

	Announcer = FWarlocksUtils::FindActor<AWarlocksAnnouncer>(GetWorld());
}

void AWarlocksGameState::MulticastResetSafeZone_Implementation(AWarlocksSafeZone* SafeZone)
{
	if (!SafeZone)
	{
		UE_LOG(LogWarlocks, Error, TEXT("MulticastResetSafeZone() called without a valid SafeZone actor found"));
		return;
	}

	SafeZone->ResetSafeZone();
}
