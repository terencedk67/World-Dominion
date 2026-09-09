using UnrealBuildTool;

public class WorldDominion : ModuleRules
{
	public WorldDominion(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"GameplayTags",
			"GameplayAbilities",
			"GameplayTasks",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"CommonUI",
			"ChaosVehicles",
			"PhysicsCore",
			"NavigationSystem",
			"Niagara",
			"Json",
			"JsonUtilities"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Slate",
			"SlateCore",
			"RenderCore"
		});

		PublicIncludePaths.AddRange(new string[]
		{
			"WorldDominion",
			"WorldDominion/Core",
			"WorldDominion/Core/Input",
			"WorldDominion/Characters",
			"WorldDominion/Government",
			"WorldDominion/Countries",
			"WorldDominion/Diplomacy",
			"WorldDominion/Economy",
			"WorldDominion/Military",
			"WorldDominion/Intelligence",
			"WorldDominion/Travel",
			"WorldDominion/Vehicles",
			"WorldDominion/Missions",
			"WorldDominion/WorldEvents",
			"WorldDominion/AI",
			"WorldDominion/UI",
			"WorldDominion/SaveSystem",
			"WorldDominion/Interaction",
			"WorldDominion/Data",
			"WorldDominion/Levels"
		});

		bEnableExceptions = true;
	}
}
