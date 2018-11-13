// Copyright 2018 GiM s.r.o. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class GFurEditor : ModuleRules
{
	public GFurEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "Slate", "InputCore", "RHI", "RenderCore", "ShaderCore",
			"UnrealEd", "EditorStyle", "VREditor", "MeshPaint" });

		PrivateDependencyModuleNames.AddRange(new string[] { "InputCore", "SlateCore", "PropertyEditor", "LevelEditor", "ViewportInteraction" });

        PublicIncludePaths.Add(ModuleDirectory + "/Public");

        PrivateIncludePaths.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Fbxsdk/include")));
        if (Target.Platform == UnrealTargetPlatform.Win32)
        {
            PublicAdditionalLibraries.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Fbxsdk/lib/x86/libfbxsdk-md.lib")));
            PublicDelayLoadDLLs.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Fbxsdk/lib/x86/libfbxsdk.dll")));
        }
        else if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Fbxsdk/lib/x64/libfbxsdk-md.lib")));
            PublicDelayLoadDLLs.Add(Path.GetFullPath(Path.Combine(ModuleDirectory, "../../Fbxsdk/lib/x64/libfbxsdk.dll")));
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
