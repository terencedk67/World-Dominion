// C++ backing for CHOOSE YOUR NATION (see the Nation Select reference image).
// Reads the full roster from UWDCountrySubsystem - it never hardcodes a
// country card. A WBP_NationSelect Blueprint child renders one card widget
// per entry in OnCountryRosterUpdated and calls SelectNation on confirm.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDCountryStats.h"
#include "WDNationSelectWidget.generated.h"

UCLASS()
class WORLDDOMINION_API UWDNationSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Level streamed in once a nation is confirmed and the player presses Start Game
	 *  (the Washington vertical slice for USA; other nations fall back gracefully -
	 *  see UWDTravelSubsystem's "not travel-ready" handling - until their own opening
	 *  levels are built). */
	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|NationSelect")
	TSoftObjectPtr<UWorld> DefaultStartLevel;

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|NationSelect")
	void OnCountryRosterUpdated(const TArray<FWDCountryStats>& Countries);

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|NationSelect")
	void OnCountryHighlighted(const FWDCountryStats& Country);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|NationSelect")
	void RefreshRoster();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|NationSelect")
	void HighlightNation(FName CountryID);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|NationSelect")
	void ConfirmNationAndStart(FName CountryID);

protected:
	virtual void NativeConstruct() override;
};
