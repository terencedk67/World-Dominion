using UnrealBuildTool;
using System.Collections.Generic;

public class WorldDominionTarget : TargetRules
{
	public WorldDominionTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "WorldDominion" });
	}
}
