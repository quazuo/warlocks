// Fill out your copyright notice in the Description page of Project Settings.

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
