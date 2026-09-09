// Thin convenience wrapper so a diplomatic NPC/leader actor (see
// IWDDiplomaticActor) can query/modify its country's standing with the
// player's nation without every Blueprint having to fetch
// UWDDiplomacySubsystem and juggle two FName country IDs by hand. The
// subsystem remains the single source of truth; this component never caches
// state of its own.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WDRelationshipTypes.h"
#include "WDRelationshipComponent.generated.h"

UCLASS(ClassGroup = (WorldDominion), meta = (BlueprintSpawnableComponent))
class WORLDDOMINION_API UWDRelationshipComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/** The country this actor represents, e.g. "China" for a Chinese ambassador NPC. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Diplomacy")
	FName RepresentedCountryID;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Diplomacy")
	FWDRelationshipState GetRelationshipWithPlayerCountry() const;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Diplomacy")
	float ModifyRelationshipWithPlayerCountry(FName Dimension, float Delta);
};
