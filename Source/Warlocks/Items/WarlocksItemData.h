#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WarlocksItemData.generated.h"

USTRUCT(BlueprintType)
struct FWarlocksItemData : public FTableRowBase
{
	GENERATED_BODY()
	
	FWarlocksItemData()
	: Name("")
	, Description("")
	, FlavorText("")
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FString FlavorText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	TSoftObjectPtr<UTexture2D> Icon;
};