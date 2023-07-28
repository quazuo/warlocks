#pragma once

#include "CoreMinimal.h"
#include "Internationalization/StringTableCore.h"
#include "WarlocksAnnouncer.generated.h"

UCLASS(Blueprintable)
class WARLOCKS_API UWarlocksAnnouncer final : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void AnnouncePlayerDeath(const FText PlayerName);

	UFUNCTION(BlueprintCallable)
	void AnnouncePlayerRoundVictory(const FText PlayerName);
	
	UFUNCTION(BlueprintCallable)
	void AnnounceSafeZoneShrink();

	UFUNCTION(BlueprintCallable)
	bool IsAnnouncement() const;

	UFUNCTION(BlueprintCallable)
	FText GetAnnouncement();

protected:
	UPROPERTY(EditDefaultsOnly)
	UStringTable* StringTable;
	
	void Announce(const FString& Key);
	void Announce(const FString& Key, const FFormatNamedArguments& NamedArguments);
	
	TQueue<FText> AnnouncementQueue;
};
