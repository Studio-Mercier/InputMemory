using UnrealBuildTool;

public class DeusGoLookEditor : ModuleRules
{
	public DeusGoLookEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"DeusGoLook",
			"DeveloperSettings",
			"PropertyEditor",
			"Settings",
			"Slate",
			"SlateCore",
			"ToolMenus",
			"UnrealEd",
			"WorkspaceMenuStructure"
		});
	}
}
