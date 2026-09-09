#include "WDNationSelectWidget.h"
#include "WDCountrySubsystem.h"
#include "WDGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UWDNationSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshRoster();
}

void UWDNationSelectWidget::RefreshRoster()
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>())
		{
			TArray<FWDCountryStats> Roster;
			for (FName CountryID : Countries->GetAllCountryIDs())
			{
				FWDCountryStats Stats;
				if (Countries->GetCountryStats(CountryID, Stats))
				{
					Roster.Add(Stats);
				}
			}
			OnCountryRosterUpdated(Roster);
		}
	}
}

void UWDNationSelectWidget::HighlightNation(FName CountryID)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>())
		{
			FWDCountryStats Stats;
			if (Countries->GetCountryStats(CountryID, Stats))
			{
				OnCountryHighlighted(Stats);
			}
		}
	}
}

void UWDNationSelectWidget::ConfirmNationAndStart(FName CountryID)
{
	if (UWDGameInstance* GI = Cast<UWDGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		GI->SetSelectedCountry(CountryID);
	}

	if (!DefaultStartLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DefaultStartLevel);
	}
}
