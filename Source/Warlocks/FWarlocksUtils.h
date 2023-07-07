

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class WARLOCKS_API FWarlocksUtils
{
public:
	static TArray<FRotator> GetSpreadRotators(const FRotator& BaseRotation, unsigned int Count, double SpreadAngle);
};
