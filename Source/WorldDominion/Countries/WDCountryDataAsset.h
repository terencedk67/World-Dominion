// Authorable per-country reference/art data, registered with the Asset Manager
// (see DefaultGame.ini [/Script/Engine.AssetManagerSettings], PrimaryAssetType
// "Country"). A content author creates one instance per nation in-editor under
// /Game/WorldDominion/Data/Countries/ (e.g. DA_Country_USA) and fills in the
// portrait/flag/capital-travel-node references.
//
// Until those instances are authored, UWDCountrySubsystem falls back to the
// plain-text Content/WorldDominion/Data/Countries.csv bootstrap so the game is
// playable/testable without an editor pass - see WDCountrySubsystem.h.
//
// Nation Select and every other widget/menu must read countries through
// UWDCountrySubsystem (asset-backed or CSV-backed), never hardcode a country
// into a widget.
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WDCountryStats.h"
#include "WDCountryDataAsset.generated.h"

UCLASS(BlueprintType)
class WORLDDOMINION_API UWDCountryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country")
	FName CountryID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country")
	FText Tagline; // e.g. "Strength Through Opportunity" from the Nation Select reference

	/** PLACEHOLDER: none shipped. FINAL ASSET REQUIRED: national flag icon texture. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country|Art")
	TSoftObjectPtr<UTexture2D> FlagTexture;

	/** PLACEHOLDER: none shipped. FINAL ASSET REQUIRED: cinematic leader portrait render. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country|Art")
	TSoftObjectPtr<UTexture2D> LeaderPortrait;

	/** Soft level reference for the capital-city destination streamed in by UWDTravelSubsystem. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country|Travel")
	TSoftObjectPtr<UWorld> CapitalLevel;

	/** Starting simulation values seeded into UWDCountrySubsystem's live FWDCountryStats
	 *  the first time this country is loaded in a new game. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Country")
	FWDCountryStats DefaultStats;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(FPrimaryAssetType(TEXT("Country")), CountryID);
	}
};
