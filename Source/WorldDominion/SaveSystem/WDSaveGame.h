// World state snapshot. Phase 1 ships the container and the fields the brief
// calls out; UWDSaveSubsystem's Save/Load wiring into every gameplay
// subsystem (country stats, relationships, mission progress, NPC memory...)
// deepens through Phase 3-4 as those subsystems' data shapes stabilize.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "WDCountryStats.h"
#include "WDMissionData.h"
#include "WDSaveGame.generated.h"

UCLASS()
class WORLDDOMINION_API UWDSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	static const FString DefaultSlotName;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FName PlayerCountryID;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FName PlayerLeaderName;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	FName CurrentLocationCountryID;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	TArray<FWDCountryStats> CountrySnapshots;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	TArray<FWDMissionDefinition> ActiveMissionSnapshots;

	/** In-game calendar minutes elapsed since new game start (see the brief's day/date/
	 *  schedule system) - a full UWDTimeSubsystem lands alongside the scheduling UI in a
	 *  later phase; the field is reserved here so save data doesn't need to be re-versioned. */
	UPROPERTY(BlueprintReadWrite, Category = "Save")
	int64 GameTimeMinutes = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Save")
	int32 SaveVersion = 1;
};
