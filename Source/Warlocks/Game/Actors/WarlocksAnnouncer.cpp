#include "WarlocksAnnouncer.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableRegistry.h"
#include "Warlocks/Warlocks.h"

void UWarlocksAnnouncer::AnnouncePlayerDeath(const FText PlayerName)
{
	if (!StringTable)
	{
		UE_LOG(LogWarlocks, Error, TEXT("No StringTable set for the Announcer"));
		return;
	}

	const FString StringTableId = StringTable->GetStringTableId().ToString();
	const FText Text = FText::FromStringTable(StringTable->GetStringTableId(), "PlayerDeath");
	const FFormatNamedArguments NamedArgs = {
		{"PlayerName", PlayerName}
	};

	Announce(Text, NamedArgs);
}

void UWarlocksAnnouncer::AnnounceSafeZoneShrink()
{
	if (!StringTable)
	{
		UE_LOG(LogWarlocks, Error, TEXT("No StringTable set for the Announcer"));
		return;
	}

	const FString StringTableId = StringTable->GetStringTableId().ToString();
	const FText Text = FText::FromStringTable(StringTable->GetStringTableId(), "SafeZoneShrink");

	Announce(Text);
}

void UWarlocksAnnouncer::Announce(const FText Text)
{
	AnnouncementQueue.Enqueue(Text);
}

void UWarlocksAnnouncer::Announce(const FText Text, const FFormatNamedArguments& NamedArguments)
{
	const FText FormattedText = FText::Format(Text, NamedArguments);
	AnnouncementQueue.Enqueue(FormattedText);
}

bool UWarlocksAnnouncer::IsAnnouncement() const
{
	return !AnnouncementQueue.IsEmpty();
}

FText UWarlocksAnnouncer::GetAnnouncement()
{
	if (AnnouncementQueue.IsEmpty())
		return FText();

	FText Announcement;
	AnnouncementQueue.Dequeue(Announcement);
	return Announcement;
}
