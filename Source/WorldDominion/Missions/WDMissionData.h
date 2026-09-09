// Reusable mission/objective data model. New missions are authored as data
// (a UWDMissionDataAsset in-editor, or an entry in the Missions.json
// bootstrap) composed from the fixed EWDObjectiveType vocabulary in
// WDTypes.h - never as new C++ mission classes.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "WDTypes.h"
#include "WDMissionData.generated.h"

USTRUCT(BlueprintType)
struct FWDMissionObjective
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FName ObjectiveID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	EWDObjectiveType Type = EWDObjectiveType::GoTo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective")
	bool bOptional = false;

	UPROPERTY(BlueprintReadOnly, Category = "Objective")
	EWDObjectiveState State = EWDObjectiveState::Inactive;
};

USTRUCT(BlueprintType)
struct FWDMissionDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FName MissionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FText DisplayName;

	/** e.g. "Mission.Diplomatic" - see WDGameplayTags. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FGameplayTag Category;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	TArray<FWDMissionObjective> Objectives;

	/** Mission auto-started when this one completes. NAME_None = nothing follows automatically. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mission")
	FName NextMissionID;
};

/** Runtime instance of a started mission - a definition plus live per-objective state. */
USTRUCT()
struct FWDActiveMission
{
	GENERATED_BODY()

	UPROPERTY()
	FWDMissionDefinition Definition;

	bool IsComplete() const
	{
		for (const FWDMissionObjective& Objective : Definition.Objectives)
		{
			if (!Objective.bOptional && Objective.State != EWDObjectiveState::Completed)
			{
				return false;
			}
		}
		return true;
	}
};

UCLASS(BlueprintType)
class WORLDDOMINION_API UWDMissionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mission")
	FWDMissionDefinition Definition;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("Mission")), Definition.MissionID);
	}
};
