// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AIProject : ModuleRules
{
	public AIProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"AIProject",
			"AIProject/Variant_Platforming",
			"AIProject/Variant_Platforming/Animation",
			"AIProject/Variant_Combat",
			"AIProject/Variant_Combat/AI",
			"AIProject/Variant_Combat/Animation",
			"AIProject/Variant_Combat/Gameplay",
			"AIProject/Variant_Combat/Interfaces",
			"AIProject/Variant_Combat/UI",
			"AIProject/Variant_SideScrolling",
			"AIProject/Variant_SideScrolling/AI",
			"AIProject/Variant_SideScrolling/Gameplay",
			"AIProject/Variant_SideScrolling/Interfaces",
			"AIProject/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
