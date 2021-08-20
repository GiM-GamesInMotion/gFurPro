// Copyright 2018 GiM s.r.o. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GFurEditor : ModuleRules
{
	string EnginePath
	{
		get { return Path.GetFullPath(Target.RelativeEnginePath); }
	}

	string EngineThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(EnginePath, "Source/ThirdParty/")); }
	}
	
	public GFurEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		MinFilesUsingPrecompiledHeaderOverride = 1;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "InputCore", "RHI", "RenderCore",
			"UnrealEd", "EditorStyle", "VREditor", "MeshPaint", "Projects" });

		PrivateDependencyModuleNames.AddRange(new string[] { "InputCore", "SlateCore", "PropertyEditor", "LevelEditor", "ViewportInteraction", "AlembicLib", "Projects",
            "EditorInteractiveToolsFramework", "InteractiveToolsFramework"});

        PublicIncludePaths.Add(ModuleDirectory + "/Public");

        PrivateIncludePaths.Add(Path.GetFullPath(Path.Combine(EngineThirdPartyPath, "FBX/2020.2/include/")));
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
			PublicAdditionalLibraries.Add(Path.GetFullPath(Path.Combine(EngineThirdPartyPath, "FBX/2020.2/lib/vs2017/x64/release/libfbxsdk.lib")));
			PublicDelayLoadDLLs.Add(Path.GetFullPath(Path.Combine(EngineThirdPartyPath, "FBX/2020.2/lib/vs2017/x64/release/libfbxsdk.dll")));
        }

        PrivateIncludePaths.Add(ModuleDirectory + "/Private");

        PublicDependencyModuleNames.Add("GFur");
        PublicDependencyModuleNames.Add("AssetTools");

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");
        // if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
        // {
        //		if (UEBuildConfiguration.bCompileSteamOSS == true)
        //		{
        //			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
        //		}
        // }
    }
}
