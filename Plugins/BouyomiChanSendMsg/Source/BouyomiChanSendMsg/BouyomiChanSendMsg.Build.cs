// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class BouyomiChanSendMsg : ModuleRules
	{
		public BouyomiChanSendMsg(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
					"BouyomiChanSendMsg/Public",
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
                new string[] {
					"BouyomiChanSendMsg/Private",
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
                    "Sockets",
                    "Networking"
					// ... add other public dependencies that you statically link with here ...
				}
				);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}