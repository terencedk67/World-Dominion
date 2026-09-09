#include "WDMotorcadeManager.h"
#include "WorldDominion.h"
#include "WDPresidentialVehicle.h"
#include "WDPresidentialSecurityComponent.h"
#include "WDSecurityAgent.h"
#include "GameFramework/Pawn.h"

AWDMotorcadeManager::AWDMotorcadeManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWDMotorcadeManager::BeginDeparture(APawn* Protectee, UWDPresidentialSecurityComponent* Security, const TArray<FVector>& RouteWaypoints)
{
	if (!LeadVehicle)
	{
		UE_LOG(LogWorldDominion, Error, TEXT("AWDMotorcadeManager::BeginDeparture - no LeadVehicle assigned."));
		return;
	}

	CurrentProtectee = Protectee;
	CurrentSecurity = Security;

	// Security boards first: fill the lead vehicle's detail seats, then overflow to escorts.
	if (Security)
	{
		for (AWDSecurityAgent* Agent : Security->GetDetail())
		{
			if (!Agent)
			{
				continue;
			}
			if (LeadVehicle->HasFreeSeat_Implementation(EWDVehicleSeatType::SecurityDetail))
			{
				IWDVehicleInteractable::Execute_EnterVehicle(LeadVehicle, Agent, EWDVehicleSeatType::SecurityDetail);
				continue;
			}
			for (AWDPresidentialVehicle* Escort : EscortVehicles)
			{
				if (Escort && Escort->HasFreeSeat_Implementation(EWDVehicleSeatType::SecurityDetail))
				{
					IWDVehicleInteractable::Execute_EnterVehicle(Escort, Agent, EWDVehicleSeatType::SecurityDetail);
					break;
				}
			}
		}
	}

	if (Protectee)
	{
		IWDVehicleInteractable::Execute_EnterVehicle(LeadVehicle, Protectee, EWDVehicleSeatType::Protectee);
	}

	LeadVehicle->MoveAlongWaypoints(RouteWaypoints);
	bFormationActive = true;
	bArrivalPending = false;

	OnMotorcadeDeparted.Broadcast();
}

void AWDMotorcadeManager::BeginArrival(const TArray<FVector>& RouteWaypoints)
{
	if (!LeadVehicle)
	{
		return;
	}

	bArrivalPending = true;
	bFormationActive = true;
	LeadVehicle->OnVehicleArrived.AddUniqueDynamic(this, &AWDMotorcadeManager::HandleLeadVehicleArrived);
	LeadVehicle->MoveAlongWaypoints(RouteWaypoints);
}

void AWDMotorcadeManager::HandleLeadVehicleArrived()
{
	if (!bArrivalPending)
	{
		return;
	}
	bArrivalPending = false;

	// Security clears the vehicles and takes up position before the protectee steps out.
	if (UWDPresidentialSecurityComponent* Security = CurrentSecurity.Get())
	{
		for (AWDSecurityAgent* Agent : Security->GetDetail())
		{
			if (!Agent)
			{
				continue;
			}
			IWDVehicleInteractable::Execute_ExitVehicle(LeadVehicle, Agent);
			for (AWDPresidentialVehicle* Escort : EscortVehicles)
			{
				if (Escort)
				{
					IWDVehicleInteractable::Execute_ExitVehicle(Escort, Agent);
				}
			}
			Agent->ResumeFollowing();
		}
	}

	DisembarkProtectee(CurrentProtectee.Get());
	OnMotorcadeArrived.Broadcast();
}

void AWDMotorcadeManager::DisembarkProtectee(APawn* Protectee)
{
	if (LeadVehicle && Protectee)
	{
		IWDVehicleInteractable::Execute_ExitVehicle(LeadVehicle, Protectee);
	}
}

void AWDMotorcadeManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bFormationActive || !LeadVehicle)
	{
		return;
	}

	const FTransform LeadTransform = LeadVehicle->GetActorTransform();
	for (int32 Index = 0; Index < EscortVehicles.Num(); ++Index)
	{
		AWDPresidentialVehicle* Escort = EscortVehicles[Index];
		if (!Escort)
		{
			continue;
		}
		const FVector Offset = EscortFormationOffsets.IsValidIndex(Index) ? EscortFormationOffsets[Index] : FVector(-400.f * (Index + 1), 0.f, 0.f);
		Escort->SetActorLocation(LeadTransform.TransformPosition(Offset));
		Escort->SetActorRotation(LeadTransform.GetRotation());
	}
}
