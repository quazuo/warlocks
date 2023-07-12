#include "WarlocksGameState.h"

void AWarlocksGameState::Announce_Implementation(const FString& Text)
{
	AnnouncementQueue.Enqueue(Text);

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, TEXT("heyy"));
}

bool AWarlocksGameState::IsAnnouncement() const
{
	return !AnnouncementQueue.IsEmpty();
}

FString AWarlocksGameState::GetAnnouncement()
{
	if (AnnouncementQueue.IsEmpty())
		return "";

	FString Announcement;
	AnnouncementQueue.Dequeue(Announcement);
	return Announcement;
}
