// Phase 1/2 scaffold. Establishes where military simulation (deployments,
// force posture, casualties feeding public approval) will live once Phase 4
// ("military, elections, advanced AI") is underway. Deliberately minimal for
// now: MilitaryPower itself already lives on FWDCountryStats and is
// modifiable via the generic FWDStatEffect pipeline (UWDEconomySubsystem::
// ApplyStatEffects), so missions/events can already reference it without
// waiting on this subsystem to be filled in.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDMilitarySubsystem.generated.h"

USTRUCT(BlueprintType)
struct FWDMilitaryDeployment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Military")
	FName CountryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Military")
	FName RegionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Military")
	float ForceStrength = 0.f;
};

UCLASS()
class WORLDDOMINION_API UWDMilitarySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Military")
	void DeployForces(FName CountryID, FName RegionID, float ForceStrength);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Military")
	TArray<FWDMilitaryDeployment> GetDeploymentsForCountry(FName CountryID) const;

protected:
	UPROPERTY(Transient)
	TArray<FWDMilitaryDeployment> Deployments;
};
