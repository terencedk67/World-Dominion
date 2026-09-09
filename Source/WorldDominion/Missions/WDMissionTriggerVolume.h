// Reusable level-placed volume: the player overlapping it completes an
// objective and, optionally, fires a world event (e.g. walking into the
// Situation Room completes "EnterSituationRoom" and triggers
// "GlobalChipShortage"). This is the generic GoTo/VisitLocation objective
// backend - no per-mission C++ needed to wire a location into the mission
// system.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWDMissionTarget.h"
#include "WDMissionTriggerVolume.generated.h"

class UBoxComponent;

UCLASS()
class WORLDDOMINION_API AWDMissionTriggerVolume : public AActor, public IWDMissionTarget
{
	GENERATED_BODY()

public:
	AWDMissionTriggerVolume();

	/** Every one of these objective IDs is completed the moment a player pawn overlaps this
	 *  volume - e.g. "entering the room" and "the meeting begins" firing together. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Mission")
	TArray<FName> ObjectiveIDs;

	/** Optional WorldEvent EventID (see UWDWorldEventSubsystem) fired the moment this
	 *  volume completes its objective - e.g. entering the Situation Room surfaces the crisis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Mission")
	FName EventIDToTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Mission")
	bool bTriggerOnce = true;

	// IWDMissionTarget
	virtual FName GetObjectiveID_Implementation() const override { return ObjectiveIDs.Num() > 0 ? ObjectiveIDs[0] : NAME_None; }
	virtual void NotifyObjectiveSatisfied_Implementation(AActor* Instigator) override {}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Mission")
	TObjectPtr<UBoxComponent> TriggerBox;

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool bHasFired = false;
};
