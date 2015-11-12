// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	using System;
	using System.IO;
	using System.Collections.Generic;

	public class JoystickPlugin : ModuleRules
	{
		// UE does not copy third party dlls to the output directory automatically.
		// Link statically so you don't have to do it manually.
		private bool LinkThirdPartyStaticallyOnWindows = true;

        // tsky GetModuleFilename is obsolete --> RulesCompiler.ModuleDirectory
		private string ModulePath
		{
			get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
		}

		private string ThirdPartyPath
		{
			get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
		}

		private string BinariesPath
		{
			get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/")); }
		}

		public virtual void SetupBinaries(
			TargetInfo Target,
			ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
			ref List<string> OutExtraModuleNames
			)
		{
			//OutBuildBinaryConfigurations.Add(
				//new UEBuildBinaryConfiguration(UEBuildBinaryType.DynamicLinkLibrary, InTargetName: "SDL2.dll"));
		}

		public JoystickPlugin(TargetInfo Target)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"Slate",
					"SlateCore",
					// ... add other public dependencies that you statically link with here ...
				});

			PrivateIncludePathModuleNames.AddRange(
				new string[]
				{
					"InputDevice",
				});

			if (Target.Type == TargetRules.TargetType.Editor)
			{
				PrivateIncludePathModuleNames.AddRange(
					new string[]
					{
						"PropertyEditor",
						"ActorPickerMode",
						"DetailCustomizations",
					});

				PrivateDependencyModuleNames.AddRange(
					new string[]
					{
						"PropertyEditor",
						"DetailCustomizations",
						// ... add private dependencies that you statically link with here ...
					});
			}

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				});			

			if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
			{
                string SDL2Path = ThirdPartyPath + "SDL2/SDL2-2.0.3/";
                string SDL2LibPath = SDL2Path + "Lib/";

                PublicIncludePaths.Add(Path.Combine(SDL2Path, "include"));

                PublicAdditionalLibraries.Add(Path.Combine(SDL2LibPath, "SDL2-static.lib"));
                PublicAdditionalLibraries.Add("Version.lib");
				
			}
			else if (Target.Platform == UnrealTargetPlatform.Mac)
			{
				PublicFrameworks.Add("/Library/Frameworks/SDL2.framework");
			}
			else if (Target.Platform == UnrealTargetPlatform.Linux)
			{			
				AddThirdPartyPrivateStaticDependencies(Target, "SDL2");
			}

		}
	}

}
