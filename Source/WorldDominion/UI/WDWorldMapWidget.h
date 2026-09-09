// C++ backing for the WORLD DOMINION command-center map (see the global-map
// reference art): MAP / TRAVEL / DIPLOMACY / MILITARY / TRADE /
// INTELLIGENCE / NATIONS tabs, a country roster with capital pin positions
// (FWDCountryStats::MapPosition), a per-nation detail panel, and the ten
// diplomatic actions from the design brief (Call Leader, Request Summit,
// Visit Country, Propose Trade Deal, Form Alliance, Impose/Remove Sanctions,
// Share Intelligence, Request Military Access, Issue Public Statement).
//
// Like every other UI class in this project, this is pure backing logic: it
// reads UWDCountrySubsystem/UWDDiplomacySubsystem and exposes
// BlueprintImplementableEvents for a WBP_WorldMap Blueprint child to render
// with the premium black/gold/navy command-center look from the reference.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDCountryStats.h"
#include "WDRelationshipTypes.h"
#include "WDWorldMapWidget.generated.h"

UENUM(BlueprintType)
enum class EWDWorldMapTab : uint8
{
	Map,
	Travel,
	Diplomacy,
	Military,
	Trade,
	Intelligence,
	Nations
};

/** One roster entry for the map/nations list - a country's live stats plus its
 *  standing relative to the player's own nation, so pins/list rows can be colored
 *  Ally/Friendly/Neutral/Tense/Hostile without the widget re-deriving it itself. */
USTRUCT(BlueprintType)
struct FWDWorldMapCountryEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "WorldMap")
	FWDCountryStats Stats;

	UPROPERTY(BlueprintReadOnly, Category = "WorldMap")
	EWDRelationStanding StandingWithPlayer = EWDRelationStanding::Neutral;

	UPROPERTY(BlueprintReadOnly, Category = "WorldMap")
	bool bIsPlayerCountry = false;
};

UCLASS()
class WORLDDOMINION_API UWDWorldMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldMap")
	void SetActiveTab(EWDWorldMapTab NewTab);

	UFUNCTION(BlueprintPure, Category = "WorldDominion|WorldMap")
	EWDWorldMapTab GetActiveTab() const { return ActiveTab; }

	/** Selecting a country (a map pin or a NATIONS list row) opens its detail panel. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldMap")
	void SelectCountry(FName CountryID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|WorldMap")
	FName GetSelectedCountry() const { return SelectedCountryID; }

	/** Executes one of the ten diplomatic actions from the World Map against the
	 *  currently selected country, on behalf of the player's own nation. VisitCountry
	 *  additionally kicks off UWDTravelSubsystem::TravelTo. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldMap")
	void ExecuteDiplomaticAction(EWDDiplomaticAction Action);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldMap")
	void RefreshRoster();

	// --- Events for a WBP_WorldMap Blueprint child to render ---

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|WorldMap")
	void OnTabChanged(EWDWorldMapTab NewTab);

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|WorldMap")
	void OnRosterUpdated(const TArray<FWDWorldMapCountryEntry>& Roster);

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|WorldMap")
	void OnCountrySelected(const FWDWorldMapCountryEntry& Entry, const FWDRelationshipState& FullRelationship);

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|WorldMap")
	void OnActionResult(const FText& Headline);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleCountryStatChanged(FName CountryID, FName StatID, float NewValue);

	EWDWorldMapTab ActiveTab = EWDWorldMapTab::Map;
	FName SelectedCountryID;
};
