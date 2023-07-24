#include "WarlocksAssetManager.h"
#include "AbilitySystemGlobals.h"

UWarlocksAssetManager& UWarlocksAssetManager::Get()
{
	UWarlocksAssetManager* Singleton = Cast<UWarlocksAssetManager>(GEngine->AssetManager);
	if (Singleton)
		return *Singleton;
	
	UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be WarlocksAssetManager!"));
	return *NewObject<UWarlocksAssetManager>();	 // never calls this
}

void UWarlocksAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
