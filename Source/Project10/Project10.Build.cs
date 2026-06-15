// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Project10 : ModuleRules
{
	public Project10(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"Test",
            "UMG",  
            "Slate",    
            "SlateCore"  
		});


	}


}
