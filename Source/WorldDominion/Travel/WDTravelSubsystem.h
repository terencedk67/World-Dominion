// Tracks where the player currently is in the world and moves them between
// destination levels (Washington -> Andrews -> in-flight -> London, etc.)
// via level streaming/travel rather than simulating continuous Earth
// geography. A destination's level reference comes from its
// UWDCountryDataAsset::CapitalLevel; countries only backed by the CSV
// bootstrap (no authored data asset yet) simply aren't travel-ready, which
// TravelTo reports rather than crashing on.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDTravelSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelStarted, FName, DestinationCountryID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTravelFailed, FName, DestinationCountryID);

UCLASS()
class WORLDDOMINION_API UWDTravelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Travel")
	FOnTravelStarted OnTravelStarted;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Travel")
	FOnTravelFailed OnTravelFailed;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Travel")
	FName GetCurrentCountryID() const { return CurrentCountryID; }

	/** Streams to DestinationCountryID's capital level. Phase 1: an immediate
	 *  ServerTravel/OpenLevel; a later pass hangs the boarding->flight->arrival
	 *  cinematic (see AWDMotorcadeManager + a to-be-built aircraft sequence) off
	 *  OnTravelStarted before the actual level change happens. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Travel")
	bool TravelTo(FName DestinationCountryID);

protected:
	UPROPERTY(Transient)
	FName CurrentCountryID = TEXT("USA");
};
