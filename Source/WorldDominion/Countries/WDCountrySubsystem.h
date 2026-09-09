// Single source of truth for every nation's live stats. Two data sources feed
// it, checked in this order at Initialize():
//   1. UWDCountryDataAsset instances registered with the Asset Manager under
//      the "Country" PrimaryAssetType (authored in-editor; the intended
//      long-term path once art/content exists).
//   2. A plain-text CSV bootstrap (Content/WorldDominion/Data/Countries.csv)
//      parsed at runtime with no import step required - what actually powers
//      the Phase 1 vertical slice today.
// Every consumer (Nation Select UI, World Map, HUD, missions, diplomacy) goes
// through this subsystem's FName-keyed API. Nothing outside this class and
// FWDCountryStats::ModifyNumericStat should ever branch on a specific country.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDCountryStats.h"
#include "WDCountrySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCountryStatChanged, FName, CountryID, FName, StatID, float, NewValue);

UCLASS(Config = Game)
class WORLDDOMINION_API UWDCountrySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Relative to ProjectContentDir(); see [/Script/WorldDominion.WDCountrySubsystem] in DefaultGame.ini. */
	UPROPERTY(Config)
	FString CountryBootstrapCsvPath = TEXT("WorldDominion/Data/Countries.csv");

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Country")
	FOnCountryStatChanged OnCountryStatChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Country")
	bool GetCountryStats(FName CountryID, FWDCountryStats& OutStats) const;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Country")
	void SetCountryStats(FName CountryID, const FWDCountryStats& NewStats);

	/** Applies Delta to StatID (see FWDCountryStats::ModifyNumericStat) and returns the new value. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Country")
	float ModifyStat(FName CountryID, FName StatID, float Delta);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Country")
	TArray<FName> GetAllCountryIDs() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Country")
	EWDRelationStanding GetRelationStanding(FName CountryA, FName CountryB) const;

protected:
	void LoadFromAssetManager();
	void LoadFromCsvBootstrap();
	bool ParseCsvRow(const TArray<FString>& Columns, FWDCountryStats& OutStats) const;

	UPROPERTY(Transient)
	TMap<FName, FWDCountryStats> CountryRegistry;
};
