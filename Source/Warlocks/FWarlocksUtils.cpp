#include "FWarlocksUtils.h"

#include "Warlocks.h"

TArray<FRotator> FWarlocksUtils::GetSpreadRotators(const FRotator& BaseRotation, const unsigned int Count,
                                                   const double SpreadAngle)
{
	const FRotator Step = FRotator(0, FRotator::ClampAxis(SpreadAngle), 0);
	const FRotator Offset = Step * (Count - 1) * 0.5;
	TArray<FRotator> Result;
	Result.Empty(Count);

	for (unsigned int i = 0; i < Count; i++)
	{
		Result.Add(BaseRotation + Step * i - Offset);
	}

	return Result;
}

TSubclassOf<UObject> FWarlocksUtils::GetBPClassPtr(const TCHAR* BPObjPath)
{
	const ConstructorHelpers::FObjectFinder<UBlueprint> BPSpellObject(BPObjPath);
	if (!BPSpellObject.Object)
	{
		UE_LOG(LogWarlocks, Error, TEXT("Unable to load the BP class object referenced by path: %s"), BPObjPath);
		return nullptr;
	}

	return BPSpellObject.Object->GeneratedClass;
}

TSubclassOf<UObject> FWarlocksUtils::GetBPClassPtr_Runtime(const TCHAR* BPObjPath)
{
	const FSoftObjectPath ItemToReference(BPObjPath);
	
	if (UObject* ItemObject = ItemToReference.ResolveObject())
	{
		if (const auto GeneratedBP = Cast<UBlueprint>(ItemObject))
		{
			return GeneratedBP->GeneratedClass;
		}
	}

	UE_LOG(LogWarlocks, Error, TEXT("Unable to load (at runtime) the BP class object referenced by path: %s"), BPObjPath);
	return nullptr;
}
