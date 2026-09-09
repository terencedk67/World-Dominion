// The live, simulated numbers for one nation. This is intentionally a plain
// USTRUCT (not a UObject/DataAsset) because it is mutated constantly at
// runtime by UWDEconomySubsystem / UWDDiplomacySubsystem / world events and
// needs to be cheap to copy into SaveGame data. Authored *reference* data
// (flag texture, portrait, capital city location for travel) lives in the
// companion UWDCountryDataAsset - this struct is the number-crunching half.
#pragma once

#include "CoreMinimal.h"
#include "WDTypes.h"
#include "WDCountryStats.generated.h"

USTRUCT(BlueprintType)
struct WORLDDOMINION_API FWDCountryStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	FName CountryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	FString Capital;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	EWDGovernmentType GovernmentType = EWDGovernmentType::Democracy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	FString LeaderName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	int64 Population = 0;

	/** In whole currency units (not thousands/millions) for straightforward arithmetic. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	double GDP = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	double Treasury = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country", meta = (ClampMin = "0", ClampMax = "100"))
	float MilitaryPower = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country", meta = (ClampMin = "0", ClampMax = "100"))
	float IntelligencePower = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	float DiplomaticInfluence = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country", meta = (ClampMin = "0", ClampMax = "100"))
	float PublicApproval = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country", meta = (ClampMin = "0", ClampMax = "100"))
	float Stability = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	TArray<FName> Resources;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	TArray<FName> Allies;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	TArray<FName> Rivals;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	TArray<FName> Sanctions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	TArray<FName> TradePartners;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country")
	TArray<FName> StrategicRegions;

	/** Normalized (0-1, top-left origin) position of the capital on an equirectangular
	 *  world map texture - lets WBP_WorldMap place a capital pin per nation with a plain
	 *  UV->Canvas conversion, no lat/long math needed at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Country", meta = (ClampMin = "0", ClampMax = "1"))
	FVector2D MapPosition = FVector2D(0.5f, 0.5f);

	/** Generic accessor used by UWDCountrySubsystem::ModifyStat/GetStat so decision/event
	 *  effects (FWDStatEffect::StatID) can target any numeric field by name without a
	 *  switch statement per caller. Returns false for an unrecognized StatID. */
	bool ModifyNumericStat(FName StatID, float Delta, float& OutNewValue);
	bool GetNumericStat(FName StatID, float& OutValue) const;
};
