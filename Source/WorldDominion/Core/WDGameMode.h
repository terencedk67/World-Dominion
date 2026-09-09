// Base GameMode for every WORLD DOMINION destination level (Washington, foreign
// capitals, aircraft interiors...). Wires the default Pawn/Controller/HUD/State
// classes and kicks off the level's opening mission from data instead of level
// blueprint spaghetti.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "WDGameMode.generated.h"

UCLASS()
class WORLDDOMINION_API AWDGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AWDGameMode();

	/** MissionID (matches Missions.json / a UWDMissionDataAsset PrimaryAssetId) auto-started
	 *  when this level goes live. Left blank for levels that don't push a mission on load
	 *  (e.g. MainMenu, NationSelect). Set per-level via a Blueprint child GameMode. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldDominion|Mission")
	FName OpeningMissionID;

protected:
	virtual void StartPlay() override;
};
