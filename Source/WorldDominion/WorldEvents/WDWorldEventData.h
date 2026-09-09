// A world event: a headline situation with 2-3 FWDDecisionOption branches,
// each carrying its own FWDStatEffect list and resulting media headline (see
// WDTypes.h). New events are authored as data - a UWDWorldEventDataAsset
// in-editor, or an entry in the WorldEvents.json bootstrap - never as a new
// C++ event class, so the event engine scales to "hundreds" as the brief
// requires.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WDTypes.h"
#include "WDWorldEventData.generated.h"

USTRUCT(BlueprintType)
struct FWDWorldEventDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldEvent")
	FName EventID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldEvent")
	FText Headline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldEvent")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldEvent")
	EWDEventCategory Category = EWDEventCategory::Political;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldEvent")
	TArray<FWDDecisionOption> Options;

	/** Objective ID (see FWDMissionObjective) this event's resolution satisfies, if any -
	 *  lets a MakeDecision objective (e.g. "RespondToCrisis") wire straight to an event. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldEvent")
	FName SatisfiesObjectiveID;
};

UCLASS(BlueprintType)
class WORLDDOMINION_API UWDWorldEventDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldEvent")
	FWDWorldEventDefinition Definition;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("WorldEvent")), Definition.EventID);
	}
};
