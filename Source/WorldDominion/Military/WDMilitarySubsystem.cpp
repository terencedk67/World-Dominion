#include "WDMilitarySubsystem.h"

void UWDMilitarySubsystem::DeployForces(FName CountryID, FName RegionID, float ForceStrength)
{
	FWDMilitaryDeployment Deployment;
	Deployment.CountryID = CountryID;
	Deployment.RegionID = RegionID;
	Deployment.ForceStrength = ForceStrength;
	Deployments.Add(Deployment);
}

TArray<FWDMilitaryDeployment> UWDMilitarySubsystem::GetDeploymentsForCountry(FName CountryID) const
{
	return Deployments.FilterByPredicate([CountryID](const FWDMilitaryDeployment& D) { return D.CountryID == CountryID; });
}
