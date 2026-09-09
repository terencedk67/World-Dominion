#include "WDWorldEventSubsystem.h"
#include "WorldDominion.h"
#include "WDEconomySubsystem.h"
#include "WDMissionSubsystem.h"
#include "Engine/AssetManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	EWDEventCategory ParseEventCategory(const FString& In)
	{
		if (In == TEXT("Military"))		return EWDEventCategory::Military;
		if (In == TEXT("Diplomatic"))		return EWDEventCategory::Diplomatic;
		if (In == TEXT("Environmental"))	return EWDEventCategory::Environmental;
		if (In == TEXT("Political"))		return EWDEventCategory::Political;
		if (In == TEXT("Social"))			return EWDEventCategory::Social;
		if (In == TEXT("Technological"))	return EWDEventCategory::Technological;
		if (In == TEXT("Intelligence"))		return EWDEventCategory::Intelligence;
		if (In == TEXT("Energy"))			return EWDEventCategory::Energy;
		if (In == TEXT("Trade"))			return EWDEventCategory::Trade;
		return EWDEventCategory::Economic;
	}

	TArray<FWDStatEffect> ParseEffects(const TArray<TSharedPtr<FJsonValue>>* EffectsArray)
	{
		TArray<FWDStatEffect> Effects;
		if (!EffectsArray)
		{
			return Effects;
		}

		for (const TSharedPtr<FJsonValue>& EffectValue : *EffectsArray)
		{
			const TSharedPtr<FJsonObject>* EffectObjPtr;
			if (!EffectValue->TryGetObject(EffectObjPtr))
			{
				continue;
			}
			const TSharedPtr<FJsonObject> EffectObj = *EffectObjPtr;

			FWDStatEffect Effect;
			Effect.StatID = FName(*EffectObj->GetStringField(TEXT("StatID")));

			FString TargetStr;
			if (EffectObj->TryGetStringField(TEXT("TargetCountryID"), TargetStr) && !TargetStr.IsEmpty())
			{
				Effect.TargetCountryID = FName(*TargetStr);
			}

			double Delta = 0.0;
			EffectObj->TryGetNumberField(TEXT("Delta"), Delta);
			Effect.Delta = static_cast<float>(Delta);

			Effects.Add(Effect);
		}
		return Effects;
	}
}

void UWDWorldEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadFromAssetManager();

	if (Definitions.Num() == 0)
	{
		LoadFromJsonBootstrap();
	}

	UE_LOG(LogWorldDominion, Log, TEXT("UWDWorldEventSubsystem: %d world event definitions loaded."), Definitions.Num());
}

void UWDWorldEventSubsystem::LoadFromAssetManager()
{
	if (!UAssetManager::IsInitialized())
	{
		return;
	}

	UAssetManager& Manager = UAssetManager::Get();
	TArray<FPrimaryAssetId> EventAssetIds;
	Manager.GetPrimaryAssetIdList(FPrimaryAssetType(TEXT("WorldEvent")), EventAssetIds);

	for (const FPrimaryAssetId& AssetId : EventAssetIds)
	{
		if (const UWDWorldEventDataAsset* Asset = Cast<UWDWorldEventDataAsset>(Manager.GetPrimaryAssetObject(AssetId)))
		{
			Definitions.Add(Asset->Definition.EventID, Asset->Definition);
		}
	}
}

void UWDWorldEventSubsystem::LoadFromJsonBootstrap()
{
	const FString JsonPath = FPaths::ProjectContentDir() / WorldEventBootstrapJsonPath;

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *JsonPath))
	{
		UE_LOG(LogWorldDominion, Error, TEXT("UWDWorldEventSubsystem: could not read WorldEvents.json at %s"), *JsonPath);
		return;
	}

	TArray<TSharedPtr<FJsonValue>> RootArray;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, RootArray))
	{
		UE_LOG(LogWorldDominion, Error, TEXT("UWDWorldEventSubsystem: failed to parse WorldEvents.json"));
		return;
	}

	for (const TSharedPtr<FJsonValue>& EventValue : RootArray)
	{
		const TSharedPtr<FJsonObject>* EventObjPtr;
		if (!EventValue->TryGetObject(EventObjPtr))
		{
			continue;
		}
		const TSharedPtr<FJsonObject> EventObj = *EventObjPtr;

		FWDWorldEventDefinition Definition;
		Definition.EventID = FName(*EventObj->GetStringField(TEXT("EventID")));
		Definition.Headline = FText::FromString(EventObj->GetStringField(TEXT("Headline")));
		Definition.Description = FText::FromString(EventObj->GetStringField(TEXT("Description")));
		Definition.Category = ParseEventCategory(EventObj->GetStringField(TEXT("Category")));

		FString ObjectiveStr;
		if (EventObj->TryGetStringField(TEXT("SatisfiesObjectiveID"), ObjectiveStr) && !ObjectiveStr.IsEmpty())
		{
			Definition.SatisfiesObjectiveID = FName(*ObjectiveStr);
		}

		const TArray<TSharedPtr<FJsonValue>>* OptionsArray;
		if (EventObj->TryGetArrayField(TEXT("Options"), OptionsArray))
		{
			for (const TSharedPtr<FJsonValue>& OptionValue : *OptionsArray)
			{
				const TSharedPtr<FJsonObject>* OptionObjPtr;
				if (!OptionValue->TryGetObject(OptionObjPtr))
				{
					continue;
				}
				const TSharedPtr<FJsonObject> OptionObj = *OptionObjPtr;

				FWDDecisionOption Option;
				Option.Label = FText::FromString(OptionObj->GetStringField(TEXT("Label")));
				Option.Description = FText::FromString(OptionObj->GetStringField(TEXT("Description")));
				Option.ResultingHeadline = FText::FromString(OptionObj->GetStringField(TEXT("ResultingHeadline")));

				const TArray<TSharedPtr<FJsonValue>>* EffectsArray;
				OptionObj->TryGetArrayField(TEXT("Effects"), EffectsArray);
				Option.Effects = ParseEffects(EffectsArray);

				Definition.Options.Add(Option);
			}
		}

		Definitions.Add(Definition.EventID, Definition);
	}
}

bool UWDWorldEventSubsystem::TriggerEvent(FName EventID)
{
	if (!Definitions.Contains(EventID))
	{
		UE_LOG(LogWorldDominion, Warning, TEXT("UWDWorldEventSubsystem::TriggerEvent - unknown EventID %s"), *EventID.ToString());
		return false;
	}

	PendingEventIDs.Add(EventID);
	OnWorldEventTriggered.Broadcast(Definitions[EventID]);
	return true;
}

bool UWDWorldEventSubsystem::ResolveEvent(FName EventID, int32 ChosenOptionIndex, FName ActingCountryID)
{
	const FWDWorldEventDefinition* Definition = Definitions.Find(EventID);
	if (!Definition || !Definition->Options.IsValidIndex(ChosenOptionIndex))
	{
		return false;
	}

	const FWDDecisionOption& Option = Definition->Options[ChosenOptionIndex];

	if (UWDEconomySubsystem* Economy = GetGameInstance()->GetSubsystem<UWDEconomySubsystem>())
	{
		Economy->ApplyStatEffects(Option.Effects, ActingCountryID);
	}

	if (!Definition->SatisfiesObjectiveID.IsNone())
	{
		if (UWDMissionSubsystem* Missions = GetGameInstance()->GetSubsystem<UWDMissionSubsystem>())
		{
			Missions->CompleteObjective(Definition->SatisfiesObjectiveID);
		}
	}

	PendingEventIDs.Remove(EventID);
	OnWorldEventResolved.Broadcast(EventID, ChosenOptionIndex, Option.ResultingHeadline);
	UE_LOG(LogWorldDominion, Log, TEXT("WorldEvent resolved: %s -> %s"), *EventID.ToString(), *Option.ResultingHeadline.ToString());

	return true;
}

bool UWDWorldEventSubsystem::GetPendingEvent(FName EventID, FWDWorldEventDefinition& OutDefinition) const
{
	if (const FWDWorldEventDefinition* Found = Definitions.Find(EventID))
	{
		OutDefinition = *Found;
		return true;
	}
	return false;
}
