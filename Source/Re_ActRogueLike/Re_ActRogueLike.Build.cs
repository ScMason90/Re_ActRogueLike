// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Re_ActRogueLike : ModuleRules
{
	public Re_ActRogueLike(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"AIModule", "Niagara", "GameplayTasks", "UMG", "GameplayTags", "NavigationSystem", "DeveloperSettings",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
