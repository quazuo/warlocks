#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WarlocksSpellData.generated.h"

USTRUCT(BlueprintType)
struct FWarlocksSpellData : public FTableRowBase
{
	GENERATED_BODY()
	
	FWarlocksSpellData()
	: Name("")
	, Description("")
	, FlavorText("")
	{}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spell)
	FString Name;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spell)
	FString Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spell)
	FString FlavorText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spell)
	TSoftObjectPtr<UTexture2D> Icon;
};