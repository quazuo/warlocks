


#include "FWarlocksUtils.h"

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
