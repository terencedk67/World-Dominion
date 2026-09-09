// Level-placed convenience actor: drop one into a level (e.g. the White
// House Situation Room) and wire a trigger volume / mission objective /
// Blueprint event to TriggerNamedEvent so designers don't need to fetch
// UWDWorldEventSubsystem by hand. Also the natural home for a future
// timer-driven "hundreds of world events" random pacing pass (Phase 4) -
// left as a stub hook (StartRandomEventPacing) for now.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WDWorldEventManager.generated.h"

UCLASS()
class WORLDDOMINION_API AWDWorldEventManager : public AActor
{
	GENERATED_BODY()

public:
	AWDWorldEventManager();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldEvents")
	bool TriggerNamedEvent(FName EventID);

	/** Phase 4 stub: periodically triggers a random event from a weighted pool. No-op today. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldEvents")
	void StartRandomEventPacing(float MinIntervalSeconds, float MaxIntervalSeconds);
};
