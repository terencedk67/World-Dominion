#include "WDCountrySubsystem.h"
#include "WorldDominion.h"
#include "WDCountryDataAsset.h"
#include "Engine/AssetManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

namespace
{
	EWDGovernmentType ParseGovernmentType(const FString& In)
	{
		if (In == TEXT("Monarchy"))		return EWDGovernmentType::Monarchy;
		if (In == TEXT("Authoritarian"))	return EWDGovernmentType::Authoritarian;
		if (In == TEXT("OneParty"))			return EWDGovernmentType::OneParty;
		if (In == TEXT("Federation"))		return EWDGovernmentType::Federation;
		return EWDGovernmentType::Democracy;
	}

	TArray<FName> ParseNameList(const FString& In)
	{
		TArray<FName> Out;
		if (In.IsEmpty())
		{
			return Out;
		}
		TArray<FString> Pieces;
		In.ParseIntoArray(Pieces, TEXT("|"), true);
		for (const FString& Piece : Pieces)
		{
			Out.Add(FName(*Piece.TrimStartAndEnd()));
		}
		return Out;
	}
}

void UWDCountrySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadFromAssetManager();

	if (CountryRegistry.Num() == 0)
	{
		LoadFromCsvBootstrap();
	}

	UE_LOG(LogWorldDominion, Log, TEXT("UWDCountrySubsystem: %d countries loaded."), CountryRegistry.Num());
}

void UWDCountrySubsystem::LoadFromAssetManager()
{
	if (!UAssetManager::IsInitialized())
	{
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	TArray<FPrimaryAssetId> CountryAssetIds;
	Manager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("Country")), CountryAssetIds);

	for (const FPrimaryAssetId& AssetId : CountryAssetIds)
	{
		if (const UWDCountryDataAsset* Asset = Cast<UWDCountryDataAsset>(Manager.GetPrimaryAssetObject(AssetId)))
		{
			FWDCountryStats Stats = Asset->DefaultStats;
			Stats.CountryID = Asset->CountryID;
			if (Stats.DisplayName.IsEmpty())
			{
				Stats.DisplayName = Asset->DisplayName.ToString();
			}
			CountryRegistry.Add(Asset->CountryID, Stats);
		}
	}
}

void UWDCountrySubsystem::LoadFromCsvBootstrap()
{
	const FString CsvPath = FPaths::ProjectContentDir() / CountryBootstrapCsvPath;

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *CsvPath))
	{
		UE_LOG(LogWorldDominion, Error, TEXT("UWDCountrySubsystem: could not read country bootstrap CSV at %s"), *CsvPath);
		return;
	}

	for (int32 LineIndex = 1; LineIndex < Lines.Num(); ++LineIndex) // skip header row
	{
		const FString& Line = Lines[LineIndex];
		if (Line.TrimStartAndEnd().IsEmpty())
		{
			continue;
		}

		TArray<FString> Columns;
		Line.ParseIntoArray(Columns, TEXT(","), false);

		FWDCountryStats Stats;
		if (ParseCsvRow(Columns, Stats))
		{
			CountryRegistry.Add(Stats.CountryID, Stats);
		}
		else
		{
			UE_LOG(LogWorldDominion, Warning, TEXT("UWDCountrySubsystem: skipped malformed CSV row %d"), LineIndex);
		}
	}
}

bool UWDCountrySubsystem::ParseCsvRow(const TArray<FString>& Columns, FWDCountryStats& OutStats) const
{
	// CountryID,DisplayName,Capital,GovernmentType,LeaderName,Population,GDP,Treasury,
	// MilitaryPower,IntelligencePower,DiplomaticInfluence,PublicApproval,Stability,
	// Resources,Allies,Rivals,Sanctions,TradePartners,StrategicRegions
	if (Columns.Num() < 19)
	{
		return false;
	}

	OutStats.CountryID = FName(*Columns[0].TrimStartAndEnd());
	OutStats.DisplayName = Columns[1].TrimStartAndEnd();
	OutStats.Capital = Columns[2].TrimStartAndEnd();
	OutStats.GovernmentType = ParseGovernmentType(Columns[3].TrimStartAndEnd());
	OutStats.LeaderName = Columns[4].TrimStartAndEnd();
	OutStats.Population = FCString::Atoi64(*Columns[5]);
	OutStats.GDP = FCString::Atod(*Columns[6]);
	OutStats.Treasury = FCString::Atod(*Columns[7]);
	OutStats.MilitaryPower = FCString::Atof(*Columns[8]);
	OutStats.IntelligencePower = FCString::Atof(*Columns[9]);
	OutStats.DiplomaticInfluence = FCString::Atof(*Columns[10]);
	OutStats.PublicApproval = FCString::Atof(*Columns[11]);
	OutStats.Stability = FCString::Atof(*Columns[12]);
	OutStats.Resources = ParseNameList(Columns[13]);
	OutStats.Allies = ParseNameList(Columns[14]);
	OutStats.Rivals = ParseNameList(Columns[15]);
	OutStats.Sanctions = ParseNameList(Columns[16]);
	OutStats.TradePartners = ParseNameList(Columns[17]);
	OutStats.StrategicRegions = ParseNameList(Columns[18]);

	return !OutStats.CountryID.IsNone();
}

bool UWDCountrySubsystem::GetCountryStats(FName CountryID, FWDCountryStats& OutStats) const
{
	if (const FWDCountryStats* Found = CountryRegistry.Find(CountryID))
	{
		OutStats = *Found;
		return true;
	}
	return false;
}

void UWDCountrySubsystem::SetCountryStats(FName CountryID, const FWDCountryStats& NewStats)
{
	CountryRegistry.Add(CountryID, NewStats);
}

float UWDCountrySubsystem::ModifyStat(FName CountryID, FName StatID, float Delta)
{
	FWDCountryStats* Stats = CountryRegistry.Find(CountryID);
	if (!Stats)
	{
		return 0.f;
	}

	float NewValue = 0.f;
	if (Stats->ModifyNumericStat(StatID, Delta, NewValue))
	{
		OnCountryStatChanged.Broadcast(CountryID, StatID, NewValue);
	}
	return NewValue;
}

TArray<FName> UWDCountrySubsystem::GetAllCountryIDs() const
{
	TArray<FName> Ids;
	CountryRegistry.GetKeys(Ids);
	return Ids;
}

EWDRelationStanding UWDCountrySubsystem::GetRelationStanding(FName CountryA, FName CountryB) const
{
	const FWDCountryStats* StatsA = CountryRegistry.Find(CountryA);
	if (!StatsA)
	{
		return EWDRelationStanding::Neutral;
	}

	if (StatsA->Allies.Contains(CountryB))	return EWDRelationStanding::Ally;
	if (StatsA->Rivals.Contains(CountryB))	return EWDRelationStanding::Hostile;
	if (StatsA->Sanctions.Contains(CountryB)) return EWDRelationStanding::Tense;
	if (StatsA->TradePartners.Contains(CountryB)) return EWDRelationStanding::Friendly;
	return EWDRelationStanding::Neutral;
}
