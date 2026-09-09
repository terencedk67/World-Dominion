#include "WDWorldMapWidget.h"
#include "WDCountrySubsystem.h"
#include "WDDiplomacySubsystem.h"
#include "WDTravelSubsystem.h"
#include "WDGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UWDWorldMapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>())
		{
			Countries->OnCountryStatChanged.AddDynamic(this, &UWDWorldMapWidget::HandleCountryStatChanged);
		}
	}

	RefreshRoster();
	OnTabChanged(ActiveTab);
}

void UWDWorldMapWidget::SetActiveTab(EWDWorldMapTab NewTab)
{
	if (ActiveTab == NewTab)
	{
		return;
	}
	ActiveTab = NewTab;
	OnTabChanged(ActiveTab);
}

void UWDWorldMapWidget::RefreshRoster()
{
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI)
	{
		return;
	}

	UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>();
	if (!Countries)
	{
		return;
	}

	const UWDGameInstance* WDGI = Cast<UWDGameInstance>(GI);
	const FName PlayerCountryID = WDGI ? WDGI->GetSelectedCountry() : NAME_None;

	TArray<FWDWorldMapCountryEntry> Roster;
	for (FName CountryID : Countries->GetAllCountryIDs())
	{
		FWDWorldMapCountryEntry Entry;
		if (!Countries->GetCountryStats(CountryID, Entry.Stats))
		{
			continue;
		}
		Entry.bIsPlayerCountry = (CountryID == PlayerCountryID);
		Entry.StandingWithPlayer = Entry.bIsPlayerCountry
			? EWDRelationStanding::Ally
			: Countries->GetRelationStanding(PlayerCountryID, CountryID);
		Roster.Add(Entry);
	}

	OnRosterUpdated(Roster);

	if (!SelectedCountryID.IsNone())
	{
		SelectCountry(SelectedCountryID);
	}
}

void UWDWorldMapWidget::SelectCountry(FName CountryID)
{
	SelectedCountryID = CountryID;

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI)
	{
		return;
	}

	UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>();
	if (!Countries)
	{
		return;
	}

	FWDWorldMapCountryEntry Entry;
	if (!Countries->GetCountryStats(CountryID, Entry.Stats))
	{
		return;
	}

	const UWDGameInstance* WDGI = Cast<UWDGameInstance>(GI);
	const FName PlayerCountryID = WDGI ? WDGI->GetSelectedCountry() : NAME_None;
	Entry.bIsPlayerCountry = (CountryID == PlayerCountryID);
	Entry.StandingWithPlayer = Entry.bIsPlayerCountry
		? EWDRelationStanding::Ally
		: Countries->GetRelationStanding(PlayerCountryID, CountryID);

	FWDRelationshipState FullRelationship;
	if (UWDDiplomacySubsystem* Diplomacy = GI->GetSubsystem<UWDDiplomacySubsystem>())
	{
		FullRelationship = Diplomacy->GetRelationship(PlayerCountryID, CountryID);
	}

	OnCountrySelected(Entry, FullRelationship);
}

void UWDWorldMapWidget::ExecuteDiplomaticAction(EWDDiplomaticAction Action)
{
	if (SelectedCountryID.IsNone())
	{
		return;
	}

	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	if (!GI)
	{
		return;
	}

	const UWDGameInstance* WDGI = Cast<UWDGameInstance>(GI);
	const FName PlayerCountryID = WDGI ? WDGI->GetSelectedCountry() : NAME_None;

	if (UWDDiplomacySubsystem* Diplomacy = GI->GetSubsystem<UWDDiplomacySubsystem>())
	{
		const FText Headline = Diplomacy->ApplyDiplomaticAction(PlayerCountryID, SelectedCountryID, Action);
		OnActionResult(Headline);
	}

	if (Action == EWDDiplomaticAction::VisitCountry)
	{
		if (UWDTravelSubsystem* Travel = GI->GetSubsystem<UWDTravelSubsystem>())
		{
			Travel->TravelTo(SelectedCountryID);
		}
	}

	RefreshRoster();
}

void UWDWorldMapWidget::HandleCountryStatChanged(FName CountryID, FName StatID, float NewValue)
{
	RefreshRoster();
}
