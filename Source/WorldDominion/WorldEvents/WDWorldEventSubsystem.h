// The event engine. Same two-source data pattern as Countries/Missions:
// UWDWorldEventDataAsset instances via the Asset Manager, falling back to
// Content/WorldDominion/Data/WorldEvents.json. TriggerEvent surfaces a
// decision to the player (UI module's Decision widget listens for
// OnWorldEventTriggered); ResolveEvent applies the chosen option's effects
// through UWDEconomySubsystem and republishes the result as a media headline.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDWorldEventData.h"
#include "WDWorldEventSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorldEventTriggered, FWDWorldEventDefinition, Event);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWorldEventResolved, FName, EventID, int32, ChosenOptionIndex, FText, Headline);

UCLASS(Config = Game)
class WORLDDOMINION_API UWDWorldEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Relative to ProjectContentDir(); see [/Script/WorldDominion.WDWorldEventSubsystem] in DefaultGame.ini. */
	UPROPERTY(Config)
	FString WorldEventBootstrapJsonPath = TEXT("WorldDominion/Data/WorldEvents.json");

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|WorldEvents")
	FOnWorldEventTriggered OnWorldEventTriggered;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|WorldEvents")
	FOnWorldEventResolved OnWorldEventResolved;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldEvents")
	bool TriggerEvent(FName EventID);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|WorldEvents")
	bool ResolveEvent(FName EventID, int32 ChosenOptionIndex, FName ActingCountryID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|WorldEvents")
	bool GetPendingEvent(FName EventID, FWDWorldEventDefinition& OutDefinition) const;

protected:
	void LoadFromAssetManager();
	void LoadFromJsonBootstrap();

	UPROPERTY(Transient)
	TMap<FName, FWDWorldEventDefinition> Definitions;

	UPROPERTY(Transient)
	TSet<FName> PendingEventIDs;
};
