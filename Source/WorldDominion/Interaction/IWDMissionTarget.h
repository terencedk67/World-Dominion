// Implemented by any actor that can satisfy a mission objective (a GoTo marker,
// an NPC that's the subject of a TalkTo, a room that's the subject of
// VisitLocation/AttendMeeting). Lets UWDMissionSubsystem drive objective
// completion from world events instead of the world hardcoding mission IDs.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWDMissionTarget.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UWDMissionTarget : public UInterface
{
	GENERATED_BODY()
};

class WORLDDOMINION_API IWDMissionTarget
{
	GENERATED_BODY()

public:
	/** Objective tag this actor satisfies when reached/used, e.g. "Mission.Objective.EnterSituationRoom". */
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Mission")
	FName GetObjectiveID() const;

	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Mission")
	void NotifyObjectiveSatisfied(AActor* Instigator);
};
