#include "WarlocksAnnouncer.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableRegistry.h"
#include "Warlocks/Warlocks.h"

AWarlocksAnnouncer::AWarlocksAnnouncer()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AWarlocksAnnouncer::Server_AnnouncePlayerDeath_Implementation(const FText& PlayerName)
{
	const FText Announcement = GetAnnouncementString("PlayerDeath", {
		                                                 {"PlayerName", PlayerName}
	                                                 });
	Multicast_Announce(Announcement);
}

void AWarlocksAnnouncer::Server_AnnouncePlayerRoundVictory_Implementation(const FText& PlayerName)
{
	const FText Announcement = GetAnnouncementString("PlayerRoundVictory", {
		                                                 {"PlayerName", PlayerName}
	                                                 });
	Multicast_Announce(Announcement);
}

void AWarlocksAnnouncer::Server_AnnounceSafeZoneShrink_Implementation()
{
	const FText Announcement = GetAnnouncementString("SafeZoneShrink");
	Multicast_Announce(Announcement);
}

void AWarlocksAnnouncer::Multicast_Announce_Implementation(const FText& Text)
{
	AnnouncementQueue.Enqueue(Text);
}

bool AWarlocksAnnouncer::IsAnnouncement() const
{
	return !AnnouncementQueue.IsEmpty();
}

FText AWarlocksAnnouncer::GetAnnouncement()
{
	if (AnnouncementQueue.IsEmpty())
		return FText();

	FText Announcement;
	AnnouncementQueue.Dequeue(Announcement);
	return Announcement;
}

FText AWarlocksAnnouncer::GetAnnouncementString(const FString& Key, const FFormatNamedArguments NamedArguments) const
{
	if (!StringTable)
	{
		UE_LOG(LogWarlocks, Error, TEXT("No StringTable set for the Announcer"));
		return FText::FromString("ERROR: check logs");
	}

	const FString StringTableId = StringTable->GetStringTableId().ToString();
	const FText Text = FText::FromStringTable(StringTable->GetStringTableId(), Key);
	return FText::Format(Text, NamedArguments);
}
