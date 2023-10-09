#pragma once

#include "CoreMinimal.h"
#include "Internationalization/StringTableCore.h"
#include "WarlocksAnnouncer.generated.h"

UCLASS(Blueprintable)
class WARLOCKS_API AWarlocksAnnouncer final : public AActor
{
	GENERATED_BODY()

public:
	AWarlocksAnnouncer();
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_AnnouncePlayerDeath(const FText& PlayerName);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_AnnouncePlayerRoundVictory(const FText& PlayerName);
	
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_AnnounceSafeZoneShrink();
	
	UFUNCTION(BlueprintCallable)
	bool IsAnnouncement() const;
	
	UFUNCTION(BlueprintCallable)
	FText GetAnnouncement();

protected:
	UPROPERTY(EditDefaultsOnly)
	UStringTable* StringTable;

	FText GetAnnouncementString(const FString& Key, const FFormatNamedArguments NamedArguments = {}) const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Announce(const FText& Text);
	
	TQueue<FText> AnnouncementQueue;
};
