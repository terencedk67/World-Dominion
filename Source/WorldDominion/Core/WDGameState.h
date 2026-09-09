// Replicated top-line stats for the player's active nation, mirrored from
// UWDCountrySubsystem/UWDEconomySubsystem so the HUD (UWDHUDWidget) always has
// a cheap, replication-safe source to bind to instead of subsystem lookups
// every frame.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WDGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStateStatsChanged);

UCLASS()
class WORLDDOMINION_API AWDGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AWDGameState();

	/** Pulls current values from UWDCountrySubsystem for the active country and republishes them. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|GameState")
	void RefreshFromCountrySubsystem();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TopLineStats, Category = "WorldDominion|GameState")
	FName ActiveCountryID;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TopLineStats, Category = "WorldDominion|GameState")
	float Influence = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TopLineStats, Category = "WorldDominion|GameState")
	float Treasury = 0.f;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TopLineStats, Category = "WorldDominion|GameState")
	int32 AllyCount = 0;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TopLineStats, Category = "WorldDominion|GameState")
	float PublicApproval = 0.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|GameState")
	void OnTopLineStatsChanged();

	/** External-binding equivalent of OnTopLineStatsChanged, for C++/UMG classes (e.g.
	 *  UWDHUDWidget) that need to AddDynamic rather than override a Blueprint event. */
	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|GameState")
	FOnGameStateStatsChanged OnStatsChangedDelegate;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_TopLineStats();
};
