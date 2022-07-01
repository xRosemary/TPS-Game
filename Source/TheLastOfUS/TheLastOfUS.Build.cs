// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TheLastOfUS : ModuleRules
{
	public TheLastOfUS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"HeadMountedDisplay",
			"HTTP",
			"HTTPServer",
			"JsonUtilities",
			"Json"
		});
	}
}
