// Copyright 2018 GiM s.r.o. All Rights Reserved.

using UnrealBuildTool;

public class GFur : ModuleRules
{
	public GFur(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		MinFilesUsingPrecompiledHeaderOverride = 1;

		PublicIncludePaths.Add(ModuleDirectory + "/Public");


        PrivateIncludePaths.Add(ModuleDirectory + "/Private");
		PrivateIncludePaths.Add(EngineDirectory + "/Shaders/Shared");


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects"
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
