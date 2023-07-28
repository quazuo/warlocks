#include "WarlocksGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Actors/WarlocksSafeZone.h"
#include "Warlocks/Warlocks.h"

AWarlocksGameState::AWarlocksGameState()
{
}

void AWarlocksGameState::BeginPlay()
{
	Super::BeginPlay();

	// create the announcer object, which handles announcements displayed on the screen
	if (AnnouncerClass)
	{
		Announcer = NewObject<UWarlocksAnnouncer>(this, AnnouncerClass, "WarlocksAnnouncer");
	}
	else
	{
		UE_LOG(LogWarlocks, Error, TEXT("No announcer class!"));
	}
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
