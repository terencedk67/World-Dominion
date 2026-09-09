#include "WDSaveSubsystem.h"
#include "WorldDominion.h"
#include "WDSaveGame.h"
#include "WDCountrySubsystem.h"
#include "WDMissionSubsystem.h"
#include "WDGameInstance.h"
#include "Kismet/GameplayStatics.h"

bool UWDSaveSubsystem::SaveWorldState(const FString& SlotName)
{
	const FString ResolvedSlot = SlotName.IsEmpty() ? UWDSaveGame::DefaultSlotName : SlotName;

	UWDSaveGame* SaveGame = Cast<UWDSaveGame>(UGameplayStatics::CreateSaveGameObject(UWDSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return false;
	}

	if (const UWDGameInstance* GI = Cast<UWDGameInstance>(GetGameInstance()))
	{
		SaveGame->PlayerCountryID = GI->GetSelectedCountry();
	}

	if (UWDCountrySubsystem* Countries = GetGameInstance()->GetSubsystem<UWDCountrySubsystem>())
	{
		for (FName CountryID : Countries->GetAllCountryIDs())
		{
			FWDCountryStats Stats;
			if (Countries->GetCountryStats(CountryID, Stats))
			{
				SaveGame->CountrySnapshots.Add(Stats);
			}
		}
	}

	if (UWDMissionSubsystem* Missions = GetGameInstance()->GetSubsystem<UWDMissionSubsystem>())
	{
		for (FName MissionID : Missions->GetActiveMissionIDs())
		{
			FWDMissionDefinition Definition;
			if (Missions->GetActiveMission(MissionID, Definition))
			{
				SaveGame->ActiveMissionSnapshots.Add(Definition);
			}
		}
	}

	const bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, ResolvedSlot, 0);
	UE_LOG(LogWorldDominion, Log, TEXT("UWDSaveSubsystem::SaveWorldState -> slot '%s': %s"), *ResolvedSlot, bSaved ? TEXT("OK") : TEXT("FAILED"));
	return bSaved;
}

bool UWDSaveSubsystem::LoadWorldState(const FString& SlotName)
{
	const FString ResolvedSlot = SlotName.IsEmpty() ? UWDSaveGame::DefaultSlotName : SlotName;

	UWDSaveGame* SaveGame = Cast<UWDSaveGame>(UGameplayStatics::LoadGameFromSlot(ResolvedSlot, 0));
	if (!SaveGame)
	{
		return false;
	}

	if (UWDGameInstance* GI = Cast<UWDGameInstance>(GetGameInstance()))
	{
		GI->SetSelectedCountry(SaveGame->PlayerCountryID);
	}

	if (UWDCountrySubsystem* Countries = GetGameInstance()->GetSubsystem<UWDCountrySubsystem>())
	{
		for (const FWDCountryStats& Stats : SaveGame->CountrySnapshots)
		{
			Countries->SetCountryStats(Stats.CountryID, Stats);
		}
	}

	if (UWDMissionSubsystem* Missions = GetGameInstance()->GetSubsystem<UWDMissionSubsystem>())
	{
		for (const FWDMissionDefinition& Definition : SaveGame->ActiveMissionSnapshots)
		{
			Missions->StartMission(Definition.MissionID);
			for (const FWDMissionObjective& Objective : Definition.Objectives)
			{
				Missions->SetObjectiveState(Objective.ObjectiveID, Objective.State);
			}
		}
	}

	return true;
}

bool UWDSaveSubsystem::DoesSaveExist(const FString& SlotName) const
{
	const FString ResolvedSlot = SlotName.IsEmpty() ? UWDSaveGame::DefaultSlotName : SlotName;
	return UGameplayStatics::DoesSaveGameExist(ResolvedSlot, 0);
}
