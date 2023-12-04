// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TencentOpenClass : ModuleRules
{
	public TencentOpenClass(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "GameplayTasks", "GameplayAbilities", "GameplayTags", 
			"Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
