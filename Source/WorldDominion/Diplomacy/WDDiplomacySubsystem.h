// Owns every bilateral relationship in the world and turns
// EWDDiplomaticAction calls (from the World Map, a phone call, or a mission)
// into relationship-dimension deltas, an economic knock-on (routed through
// UWDEconomySubsystem), and a media headline. Consequences are defined once
// per *action type* (see GetDefaultOutcome) rather than per country pair -
// specific countries only ever appear as the FName arguments passed in.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDRelationshipTypes.h"
#include "WDDiplomacySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRelationshipChanged, FName, CountryA, FName, CountryB, FWDRelationshipState, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiplomaticHeadline, FText, Headline);

UCLASS()
class WORLDDOMINION_API UWDDiplomacySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Diplomacy")
	FOnRelationshipChanged OnRelationshipChanged;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Diplomacy")
	FOnDiplomaticHeadline OnDiplomaticHeadline;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Diplomacy")
	FWDRelationshipState GetRelationship(FName CountryA, FName CountryB) const;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Diplomacy")
	float ModifyRelationship(FName CountryA, FName CountryB, FName Dimension, float Delta);

	/** Executes a diplomatic action from CountryA (usually the player's nation) toward
	 *  CountryB, applying its default relationship/economic consequences and returning
	 *  a media headline ready for the news system. Mirrors the "US raises China tariffs"
	 *  chain-of-consequence example from the design brief. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Diplomacy")
	FText ApplyDiplomaticAction(FName CountryA, FName CountryB, EWDDiplomaticAction Action);

protected:
	static FString MakePairKey(FName CountryA, FName CountryB);

	UPROPERTY(Transient)
	TMap<FString, FWDRelationshipState> Relationships;
};
