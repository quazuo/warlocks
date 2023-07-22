#include "WarlocksGameState.h"

void AWarlocksGameState::Announce_Implementation(const FString& Text)
{
	AnnouncementQueue.Enqueue(Text);
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
