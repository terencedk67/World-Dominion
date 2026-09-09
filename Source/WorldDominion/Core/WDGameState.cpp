#include "WDGameState.h"
#include "Net/UnrealNetwork.h"
#include "WDCountrySubsystem.h"
#include "WDCountryStats.h"
#include "Kismet/GameplayStatics.h"

AWDGameState::AWDGameState()
{
	bReplicates = true;
}

void AWDGameState::RefreshFromCountrySubsystem()
{
	if (const UGameInstance* GI = GetGameInstance())
	{
		if (UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>())
		{
			FWDCountryStats Stats;
			if (Countries->GetCountryStats(ActiveCountryID, Stats))
			{
				Influence = Stats.DiplomaticInfluence;
				Treasury = Stats.Treasury;
				AllyCount = Stats.Allies.Num();
				PublicApproval = Stats.PublicApproval;
				OnRep_TopLineStats();
			}
		}
	}
}

void AWDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWDGameState, ActiveCountryID);
	DOREPLIFETIME(AWDGameState, Influence);
	DOREPLIFETIME(AWDGameState, Treasury);
	DOREPLIFETIME(AWDGameState, AllyCount);
	DOREPLIFETIME(AWDGameState, PublicApproval);
}

void AWDGameState::OnRep_TopLineStats()
{
	OnTopLineStatsChanged();
	OnStatsChangedDelegate.Broadcast();
}
