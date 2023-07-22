// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Warlocks : ModuleRules
{
	public Warlocks(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule",
			"Niagara", "EnhancedInput"
		});

		PrivateDependencyModuleNames.AddRange(new[]
		{
			"GameplayAbilities", "GameplayTags", "GameplayTasks"
		});
	}
}