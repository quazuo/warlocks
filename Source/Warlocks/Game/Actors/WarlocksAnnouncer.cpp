#include "WarlocksAnnouncer.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableRegistry.h"
#include "Warlocks/Warlocks.h"

void UWarlocksAnnouncer::AnnouncePlayerDeath(const FText PlayerName)
{
	const FFormatNamedArguments NamedArgs = {
		{"PlayerName", PlayerName}
	};

	Announce("PlayerDeath", NamedArgs);
}

void UWarlocksAnnouncer::AnnouncePlayerRoundVictory(const FText PlayerName)
{
	const FFormatNamedArguments NamedArgs = {
		{"PlayerName", PlayerName}
	};

	Announce("PlayerRoundVictory", NamedArgs);
}

void UWarlocksAnnouncer::AnnounceSafeZoneShrink()
{
	Announce("SafeZoneShrink");
}

void UWarlocksAnnouncer::Announce(const FString& Key)
{
	if (!StringTable)
	{
		UE_LOG(LogWarlocks, Error, TEXT("No StringTable set for the Announcer"));
		return;
	}
	
	const FString StringTableId = StringTable->GetStringTableId().ToString();
	const FText Text = FText::FromStringTable(StringTable->GetStringTableId(), Key);
	
	AnnouncementQueue.Enqueue(Text);
}

void UWarlocksAnnouncer::Announce(const FString& Key, const FFormatNamedArguments& NamedArguments)
{
	if (!StringTable)
	{
		UE_LOG(LogWarlocks, Error, TEXT("No StringTable set for the Announcer"));
		return;
	}
	
	const FString StringTableId = StringTable->GetStringTableId().ToString();
	const FText Text = FText::FromStringTable(StringTable->GetStringTableId(), Key);
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
