// Persists across level travel: which nation/leader the player chose at the
// Nation Select screen, and top-line session bookkeeping. Actual simulation
// state (relationships, treasury, mission progress) lives in GameInstance
// Subsystems (UWDCountrySubsystem, UWDDiplomacySubsystem, etc.) so it survives
// travel between destination levels (Washington -> Andrews -> London, ...).
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WDGameInstance.generated.h"

UCLASS()
class WORLDDOMINION_API UWDGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Session")
	void SetSelectedCountry(FName CountryID) { SelectedCountryID = CountryID; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Session")
	FName GetSelectedCountry() const { return SelectedCountryID; }

	virtual void Init() override;

protected:
	/** e.g. "USA" - the nation chosen on the Nation Select screen. Resolved against
	 *  UWDCountrySubsystem to fetch the full FWDCountryStats / UWDCountryDataAsset. */
	UPROPERTY(BlueprintReadOnly, Category = "WorldDominion|Session")
	FName SelectedCountryID = TEXT("USA");
};
