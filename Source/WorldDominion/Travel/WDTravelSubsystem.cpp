#include "WDTravelSubsystem.h"
#include "WorldDominion.h"
#include "WDCountryDataAsset.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

bool UWDTravelSubsystem::TravelTo(FName DestinationCountryID)
{
	if (DestinationCountryID == CurrentCountryID)
	{
		return false;
	}

	if (!UAssetManager::IsInitialized())
	{
		OnTravelFailed.Broadcast(DestinationCountryID);
		return false;
	}

	UAssetManager& Manager = UAssetManager::Get();
	const FPrimaryAssetId AssetId(FPrimaryAssetType(TEXT("Country")), DestinationCountryID);
	const UWDCountryDataAsset* CountryAsset = Cast<UWDCountryDataAsset>(Manager.GetPrimaryAssetObject(AssetId));

	if (!CountryAsset || CountryAsset->CapitalLevel.IsNull())
	{
		UE_LOG(LogWorldDominion, Warning, TEXT("UWDTravelSubsystem::TravelTo - %s has no authored capital level yet (CSV-bootstrap countries are not travel-ready until a UWDCountryDataAsset is created for them)."), *DestinationCountryID.ToString());
		OnTravelFailed.Broadcast(DestinationCountryID);
		return false;
	}

	OnTravelStarted.Broadcast(DestinationCountryID);
	CurrentCountryID = DestinationCountryID;

	UGameplayStatics::OpenLevelBySoftObjectPtr(this, CountryAsset->CapitalLevel);
	return true;
}
