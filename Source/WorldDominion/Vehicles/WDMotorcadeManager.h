// Orchestrates a presidential convoy: boarding order, escort formation
// spacing along a route, and the departure/arrival sequencing described in
// the brief (security exits first on arrival, vehicles hold formation while
// moving, etc). Works with whatever AWDPresidentialVehicle instances are
// placed in the level (or spawned from the configured classes) - it does not
// know or care whether the level's road layout is Washington, London, or
// Beijing.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WDMotorcadeManager.generated.h"

class AWDPresidentialVehicle;
class UWDPresidentialSecurityComponent;
class APawn;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMotorcadeDeparted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMotorcadeArrived);

UCLASS()
class WORLDDOMINION_API AWDMotorcadeManager : public AActor
{
	GENERATED_BODY()

public:
	AWDMotorcadeManager();

	/** Lead vehicle (the President's limousine/SUV). Assign in-editor or via SpawnConvoy. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "WorldDominion|Motorcade")
	TObjectPtr<AWDPresidentialVehicle> LeadVehicle;

	/** Escort SUVs/support vehicles trailing/leading the lead vehicle in formation. */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "WorldDominion|Motorcade")
	TArray<TObjectPtr<AWDPresidentialVehicle>> EscortVehicles;

	/** Relative offset (in the lead vehicle's local space, applied continuously) each escort
	 *  keeps while the convoy is moving as one formation. Index-matched to EscortVehicles. */
	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Motorcade")
	TArray<FVector> EscortFormationOffsets;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Motorcade")
	FOnMotorcadeDeparted OnMotorcadeDeparted;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Motorcade")
	FOnMotorcadeArrived OnMotorcadeArrived;

	/** Boards Protectee + the security detail into the lead vehicle and escorts, then departs
	 *  along RouteWaypoints. Security agents board first per the brief. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Motorcade")
	void BeginDeparture(APawn* Protectee, UWDPresidentialSecurityComponent* Security, const TArray<FVector>& RouteWaypoints);

	/** Drives the convoy through RouteWaypoints to an arrival point, then plays the arrival
	 *  sequence: security exits and forms a perimeter first, then the protectee disembarks. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Motorcade")
	void BeginArrival(const TArray<FVector>& RouteWaypoints);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Motorcade")
	void DisembarkProtectee(APawn* Protectee);

protected:
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void HandleLeadVehicleArrived();

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> CurrentProtectee;

	UPROPERTY(Transient)
	TWeakObjectPtr<UWDPresidentialSecurityComponent> CurrentSecurity;

	bool bFormationActive = false;
	bool bArrivalPending = false;
};
