// Canonical entry point for applying numeric consequences to a country's
// economy (and, via FWDStatEffect's generic StatID, any other simulated
// number). World events, decisions, and diplomatic actions all route their
// effects through ApplyStatEffects instead of poking UWDCountrySubsystem
// directly, so there is one place that logs/broadcasts every economic change
// for the media system to react to.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDTypes.h"
#include "WDEconomySubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEconomicStatApplied, FName, CountryID, FName, StatID, float, NewValue);

UCLASS()
class WORLDDOMINION_API UWDEconomySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Economy")
	FOnEconomicStatApplied OnEconomicStatApplied;

	/** Applies every effect in Effects (each may target a different country) via
	 *  UWDCountrySubsystem::ModifyStat and broadcasts one notification per effect. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Economy")
	void ApplyStatEffects(const TArray<FWDStatEffect>& Effects, FName ActingCountryID);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Economy")
	bool TransferTreasuryFunds(FName FromCountryID, FName ToCountryID, float Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Economy")
	double GetTreasury(FName CountryID) const;
};
