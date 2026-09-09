#include "WDMissionSubsystem.h"
#include "WorldDominion.h"
#include "Engine/AssetManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "GameplayTagsManager.h"

namespace
{
	EWDObjectiveType ParseObjectiveType(const FString& In)
	{
		if (In == TEXT("TalkTo"))			return EWDObjectiveType::TalkTo;
		if (In == TEXT("Interact"))		return EWDObjectiveType::Interact;
		if (In == TEXT("EnterVehicle"))		return EWDObjectiveType::EnterVehicle;
		if (In == TEXT("Travel"))			return EWDObjectiveType::Travel;
		if (In == TEXT("AttendMeeting"))	return EWDObjectiveType::AttendMeeting;
		if (In == TEXT("MakeDecision"))		return EWDObjectiveType::MakeDecision;
		if (In == TEXT("Wait"))				return EWDObjectiveType::Wait;
		if (In == TEXT("Escort"))			return EWDObjectiveType::Escort;
		if (In == TEXT("ReviewDocument"))	return EWDObjectiveType::ReviewDocument;
		if (In == TEXT("GiveSpeech"))		return EWDObjectiveType::GiveSpeech;
		if (In == TEXT("VisitLocation"))	return EWDObjectiveType::VisitLocation;
		return EWDObjectiveType::GoTo;
	}
}

void UWDMissionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadFromAssetManager();

	if (Definitions.Num() == 0)
	{
		LoadFromJsonBootstrap();
	}

	UE_LOG(LogWorldDominion, Log, TEXT("UWDMissionSubsystem: %d mission definitions loaded."), Definitions.Num());
}

void UWDMissionSubsystem::LoadFromAssetManager()
{
	if (!UAssetManager::IsInitialized())
	{
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	TArray<FPrimaryAssetId> MissionAssetIds;
	Manager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("Mission")), MissionAssetIds);

	for (const FPrimaryAssetId& AssetId : MissionAssetIds)
	{
		if (const UWDMissionDataAsset* Asset = Cast<UWDMissionDataAsset>(Manager.GetPrimaryAssetObject(AssetId)))
		{
			Definitions.Add(Asset->Definition.MissionID, Asset->Definition);
		}
	}
}

void UWDMissionSubsystem::LoadFromJsonBootstrap()
{
	const FString JsonPath = FPaths::ProjectContentDir() / MissionBootstrapJsonPath;

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonPath))
	{
		UE_LOG(LogWorldDominion, Error, TEXT("UWDMissionSubsystem: could not read mission bootstrap JSON at %s"), *JsonPath);
		return;
	}

	TArray<TSharedPtr<FJsonValue>> RootArray;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, RootArray))
	{
		UE_LOG(LogWorldDominion, Error, TEXT("UWDMissionSubsystem: failed to parse Missions.json"));
		return;
	}

	for (const TSharedPtr<FJsonValue>& MissionValue : RootArray)
	{
		const TSharedPtr<FJsonObject>* MissionObjPtr;
		if (!MissionValue->TryGetObject(MissionObjPtr))
		{
			continue;
		}
		const TSharedPtr<FJsonObject> MissionObj = *MissionObjPtr;

		FWDMissionDefinition Definition;
		Definition.MissionID = FName(*MissionObj->GetStringField(TEXT("MissionID")));
		Definition.DisplayName = FText::FromString(MissionObj->GetStringField(TEXT("DisplayName")));

		FString CategoryStr;
		if (MissionObj->TryGetStringField(TEXT("Category"), CategoryStr))
		{
			Definition.Category = UGameplayTagsManager::Get().RequestGameplayTag(FName(*CategoryStr), false);
		}

		FString NextMissionStr;
		if (MissionObj->TryGetStringField(TEXT("NextMissionID"), NextMissionStr) && !NextMissionStr.IsEmpty())
		{
			Definition.NextMissionID = FName(*NextMissionStr);
		}

		const TArray<TSharedPtr<FJsonValue>>* ObjectivesArray;
		if (MissionObj->TryGetArrayField(TEXT("Objectives"), ObjectivesArray))
		{
			for (const TSharedPtr<FJsonValue>& ObjectiveValue : *ObjectivesArray)
			{
				const TSharedPtr<FJsonObject>* ObjectiveObjPtr;
				if (!ObjectiveValue->TryGetObject(ObjectiveObjPtr))
				{
					continue;
				}
				const TSharedPtr<FJsonObject> ObjectiveObj = *ObjectiveObjPtr;

				FWDMissionObjective Objective;
				Objective.ObjectiveID = FName(*ObjectiveObj->GetStringField(TEXT("ObjectiveID")));
				Objective.Type = ParseObjectiveType(ObjectiveObj->GetStringField(TEXT("Type")));
				Objective.Description = FText::FromString(ObjectiveObj->GetStringField(TEXT("Description")));
				bool bOptional = false;
				ObjectiveObj->TryGetBoolField(TEXT("Optional"), bOptional);
				Objective.bOptional = bOptional;
				Objective.State = EWDObjectiveState::Inactive;

				Definition.Objectives.Add(Objective);
			}
		}

		Definitions.Add(Definition.MissionID, Definition);
	}
}

bool UWDMissionSubsystem::StartMission(FName MissionID)
{
	const FWDMissionDefinition* Definition = Definitions.Find(MissionID);
	if (!Definition)
	{
		UE_LOG(LogWorldDominion, Warning, TEXT("UWDMissionSubsystem::StartMission - unknown MissionID %s"), *MissionID.ToString());
		return false;
	}

	FWDActiveMission Active;
	Active.Definition = *Definition;
	for (FWDMissionObjective& Objective : Active.Definition.Objectives)
	{
		Objective.State = EWDObjectiveState::Active;
	}

	ActiveMissions.Add(MissionID, Active);
	OnMissionStarted.Broadcast(MissionID);

	for (const FWDMissionObjective& Objective : Active.Definition.Objectives)
	{
		OnObjectiveStateChanged.Broadcast(Objective.ObjectiveID, EWDObjectiveState::Active);
	}

	return true;
}

void UWDMissionSubsystem::SetObjectiveState(FName ObjectiveID, EWDObjectiveState NewState)
{
	for (auto& Pair : ActiveMissions)
	{
		for (FWDMissionObjective& Objective : Pair.Value.Definition.Objectives)
		{
			if (Objective.ObjectiveID == ObjectiveID)
			{
				Objective.State = NewState;
				OnObjectiveStateChanged.Broadcast(ObjectiveID, NewState);
				CheckMissionCompletion(Pair.Key);
				return;
			}
		}
	}

	UE_LOG(LogWorldDominion, Warning, TEXT("UWDMissionSubsystem::SetObjectiveState - ObjectiveID %s not found in any active mission."), *ObjectiveID.ToString());
}

void UWDMissionSubsystem::CompleteObjective(FName ObjectiveID)
{
	SetObjectiveState(ObjectiveID, EWDObjectiveState::Completed);
}

void UWDMissionSubsystem::CheckMissionCompletion(FName MissionID)
{
	FWDActiveMission* Active = ActiveMissions.Find(MissionID);
	if (!Active || !Active->IsComplete())
	{
		return;
	}

	const FName NextMissionID = Active->Definition.NextMissionID;
	ActiveMissions.Remove(MissionID);
	OnMissionCompleted.Broadcast(MissionID);

	if (!NextMissionID.IsNone())
	{
		StartMission(NextMissionID);
	}
}

bool UWDMissionSubsystem::GetActiveMission(FName MissionID, FWDMissionDefinition& OutDefinition) const
{
	if (const FWDActiveMission* Active = ActiveMissions.Find(MissionID))
	{
		OutDefinition = Active->Definition;
		return true;
	}
	return false;
}

TArray<FName> UWDMissionSubsystem::GetActiveMissionIDs() const
{
	TArray<FName> Ids;
	ActiveMissions.GetKeys(Ids);
	return Ids;
}

TArray<FWDMissionObjective> UWDMissionSubsystem::GetAllActiveObjectives() const
{
	TArray<FWDMissionObjective> Result;
	for (const auto& Pair : ActiveMissions)
	{
		for (const FWDMissionObjective& Objective : Pair.Value.Definition.Objectives)
		{
			if (Objective.State == EWDObjectiveState::Active)
			{
				Result.Add(Objective);
			}
		}
	}
	return Result;
}
