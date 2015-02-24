// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	using System;
	using System.IO;

	public class JoystickPlugin : ModuleRules
	{
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

		private string IncludePathSDL2
		{
			get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "SDL2/SDL2", "include")); }
		}

		private string LibraryPathSDL2
		{
			get { return Path.GetFullPath(Path.Combine(ThirdPartyPath, "SDL2/SDL2", "Lib")); }
		}

		public JoystickPlugin(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[]
				{
					"JoystickPlugin/Public",
					// ... add public include paths required here ...
				});

			PrivateIncludePaths.AddRange(
				new string[]
				{
					"JoystickPlugin/Private",
				});

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


			PublicIncludePaths.Add(IncludePathSDL2);
			PublicAdditionalLibraries.Add(Path.Combine(LibraryPathSDL2, "SDL2.lib"));

			PublicDelayLoadDLLs.Add("SDL2.dll");
		}

		

	}

}
