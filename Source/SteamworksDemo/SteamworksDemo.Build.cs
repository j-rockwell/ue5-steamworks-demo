// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SteamworksDemo : ModuleRules
{
	public SteamworksDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "OnlineSubsystem" });
		PrivateDependencyModuleNames.AddRange(new string[] { "Steamworks" });
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
	}
}
