// Drives every open-world mission from data. Mirrors UWDCountrySubsystem's
// two-source loading pattern: UWDMissionDataAsset instances via the Asset
// Manager first, falling back to a runtime JSON bootstrap
// (Content/WorldDominion/Data/Missions.json) so the vertical slice's
// "Attend Emergency Summit" mission runs with no in-editor authoring step.
//
// Multiple missions can be active concurrently (a main mission plus side
// objectives), each tracked independently; ObjectiveIDs are assumed globally
// unique for a playthrough so CompleteObjective can be called from anywhere
// (an NPC's OnInteract, a trigger volume, a decision result) without knowing
// which mission owns that objective.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDMissionData.h"
#include "WDMissionSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionStarted, FName, MissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMissionCompleted, FName, MissionID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnObjectiveStateChanged, FName, ObjectiveID, EWDObjectiveState, NewState);

UCLASS(Config = Game)
class WORLDDOMINION_API UWDMissionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Relative to ProjectContentDir(); see [/Script/WorldDominion.WDMissionSubsystem] in DefaultGame.ini. */
	UPROPERTY(Config)
	FString MissionBootstrapJsonPath = TEXT("WorldDominion/Data/Missions.json");

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Mission")
	FOnMissionStarted OnMissionStarted;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Mission")
	FOnMissionCompleted OnMissionCompleted;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Mission")
	FOnObjectiveStateChanged OnObjectiveStateChanged;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Mission")
	bool StartMission(FName MissionID);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Mission")
	void CompleteObjective(FName ObjectiveID);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Mission")
	void SetObjectiveState(FName ObjectiveID, EWDObjectiveState NewState);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Mission")
	bool GetActiveMission(FName MissionID, FWDMissionDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Mission")
	TArray<FName> GetActiveMissionIDs() const;

	/** All currently Active (started, not yet completed/failed) objectives across every
	 *  running mission - what the HUD's objectives panel binds to. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Mission")
	TArray<FWDMissionObjective> GetAllActiveObjectives() const;

protected:
	void LoadFromAssetManager();
	void LoadFromJsonBootstrap();

	UPROPERTY(Transient)
	TMap<FName, FWDMissionDefinition> Definitions;

	UPROPERTY(Transient)
	TMap<FName, FWDActiveMission> ActiveMissions;

	void CheckMissionCompletion(FName MissionID);
};
