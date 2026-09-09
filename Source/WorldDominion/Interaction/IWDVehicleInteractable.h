// Implemented by presidential vehicles/aircraft the player and their security
// detail can enter/exit. Separate from IWDInteractable so vehicle prompts can
// carry seat-specific data (which door, driver vs. passenger vs. protectee).
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWDVehicleInteractable.generated.h"

UENUM(BlueprintType)
enum class EWDVehicleSeatType : uint8
{
	Driver,
	Protectee,
	SecurityDetail,
	Passenger
};

UINTERFACE(BlueprintType, MinimalAPI)
class UWDVehicleInteractable : public UInterface
{
	GENERATED_BODY()
};

class WORLDDOMINION_API IWDVehicleInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Vehicle")
	bool HasFreeSeat(EWDVehicleSeatType SeatType) const;

	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Vehicle")
	bool EnterVehicle(APawn* RequestingPawn, EWDVehicleSeatType SeatType);

	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Vehicle")
	bool ExitVehicle(APawn* RequestingPawn);

	/** True once security has cleared the vehicle and the player is permitted to drive it themself. */
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Vehicle")
	bool CanPlayerDrive() const;
};
