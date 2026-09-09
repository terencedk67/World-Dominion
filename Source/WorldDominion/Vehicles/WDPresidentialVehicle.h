// A motorcade vehicle (limousine, SUV escort, or support vehicle). Phase 1
// uses simple kinematic waypoint-following (MoveAlongWaypoints) driven by
// AWDMotorcadeManager rather than full Chaos Vehicle physics: a proper
// UChaosWheeledVehicleMovementComponent needs per-wheel bone data on a
// skeletal mesh, which does not exist yet (see PLACEHOLDER note below).
// Once a real vehicle Skeletal Mesh with wheel bones is authored, swap this
// class's movement for AWheeledVehiclePawn + UChaosVehicleWheel subclasses -
// IWDVehicleInteractable's seat contract stays identical either way, so
// nothing else in the game needs to change.
//
// PLACEHOLDER: elongated cube static mesh stand-in.
// FINAL ASSET REQUIRED: Presidential SUV / limousine mesh (Chaos Vehicle-ready
// skeletal mesh with wheel bones once real driving is implemented).
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IWDVehicleInteractable.h"
#include "WDPresidentialVehicle.generated.h"

class UStaticMeshComponent;
class USceneComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVehicleArrived);

UCLASS()
class WORLDDOMINION_API AWDPresidentialVehicle : public APawn, public IWDVehicleInteractable
{
	GENERATED_BODY()

public:
	AWDPresidentialVehicle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Placeholder")
	TObjectPtr<UStaticMeshComponent> PlaceholderBodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Vehicle")
	TObjectPtr<USceneComponent> DriverSeat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Vehicle")
	TObjectPtr<USceneComponent> ProtecteeSeat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Vehicle")
	TArray<TObjectPtr<USceneComponent>> SecuritySeats;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Vehicle")
	float CruiseSpeed = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Vehicle")
	float TurnSpeedDegPerSec = 90.f;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Vehicle")
	FOnVehicleArrived OnVehicleArrived;

	/** Sets bPlayerCanDrive; toggled by security posture (a President never drives during
	 *  a Lockdown, for example). */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Vehicle")
	void SetPlayerDrivingPermitted(bool bAllowed) { bPlayerCanDrive = bAllowed; }

	/** Kicks off kinematic travel through Waypoints in order; broadcasts OnVehicleArrived
	 *  once the last one is reached. Used by AWDMotorcadeManager for departure/arrival. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Vehicle")
	void MoveAlongWaypoints(const TArray<FVector>& Waypoints);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Vehicle")
	void StopMoving();

	// IWDVehicleInteractable
	virtual bool HasFreeSeat_Implementation(EWDVehicleSeatType SeatType) const override;
	virtual bool EnterVehicle_Implementation(APawn* RequestingPawn, EWDVehicleSeatType SeatType) override;
	virtual bool ExitVehicle_Implementation(APawn* RequestingPawn) override;
	virtual bool CanPlayerDrive_Implementation() const override { return bPlayerCanDrive; }

protected:
	virtual void Tick(float DeltaSeconds) override;

	USceneComponent* GetSeatComponent(EWDVehicleSeatType SeatType, int32 Index = 0) const;

	UPROPERTY(Transient)
	TMap<TObjectPtr<USceneComponent>, TWeakObjectPtr<APawn>> SeatOccupants;

	bool bPlayerCanDrive = false;

	TArray<FVector> ActiveWaypoints;
	int32 CurrentWaypointIndex = INDEX_NONE;
};
