#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "WarlocksAssetManager.generated.h"

UCLASS()
class WARLOCKS_API UWarlocksAssetManager final : public UAssetManager
{
	GENERATED_BODY()
	
public:

	static UWarlocksAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};