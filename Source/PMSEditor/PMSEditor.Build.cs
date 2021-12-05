// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PMSEditor : ModuleRules
{
	public PMSEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				//"Runtime/AppFramework/Private/Framework/Testing",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
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

				"EditorStyle",
				"UnrealEd",
				"AssetTools",
				"GraphEditor",
				"ApplicationCore",
				"ToolMenus",
				
				"Projects",

				//"AppFramework",

				"PMS",
				"KismetWidgets",
				"Json",
				"JsonUtilities",
				
				//From Slate
				/*InputCore是和检测输入相关的模块*/
				"InputCore",
				
				/*视图相关模块*/
				"AdvancedPreviewScene",
				
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
