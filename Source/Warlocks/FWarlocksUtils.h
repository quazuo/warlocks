#pragma once

#include "CoreMinimal.h"

class WARLOCKS_API FWarlocksUtils
{
public:
	/**
	 * Generate a number of rotators, spread by a specified angle around the Z axis.
	 * @param BaseRotation The rotation around which the generated rotators will be spread. When Count is odd,
	 * it also is the rotation which the middle rotator faces.
	 * @param Count The number of desired rotators.
	 * @param SpreadAngle The angle between generated rotators.
	 * @return Array of generated rotators.
	 */
	static TArray<FRotator> GetSpreadRotators(const FRotator& BaseRotation, unsigned int Count, double SpreadAngle);

	/**
	 * Get a pointer to a blueprint generated class. Usable only within constructors.
	 * @param BPObjPath The path (reference) to the blueprint object.
	 * @return The desired class.
	 */
	static TSubclassOf<UObject> GetBPClassPtr(const TCHAR* BPObjPath);

	/**
	 * Get a pointer to a blueprint generated class. Usable outside of constructors.
	 * @param BPObjPath The path (reference) to the blueprint object.
	 * @return The desired class.
	 */
	static TSubclassOf<UObject> GetBPClassPtr_Runtime(const TCHAR* BPObjPath);
};
