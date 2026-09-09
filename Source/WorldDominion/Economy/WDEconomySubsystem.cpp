#include "WDEconomySubsystem.h"
#include "WDCountrySubsystem.h"
#include "WDCountryStats.h"

void UWDEconomySubsystem::ApplyStatEffects(const TArray<FWDStatEffect>& Effects, FName ActingCountryID)
{
	UWDCountrySubsystem* Countries = GetGameInstance()->GetSubsystem<UWDCountrySubsystem>();
	if (!Countries)
	{
		return;
	}

	for (const FWDStatEffect& Effect : Effects)
	{
		const FName TargetCountry = Effect.TargetCountryID.IsNone() ? ActingCountryID : Effect.TargetCountryID;
		const float NewValue = Countries->ModifyStat(TargetCountry, Effect.StatID, Effect.Delta);
		OnEconomicStatApplied.Broadcast(TargetCountry, Effect.StatID, NewValue);
	}
}

bool UWDEconomySubsystem::TransferTreasuryFunds(FName FromCountryID, FName ToCountryID, float Amount)
{
	if (Amount <= 0.f || GetTreasury(FromCountryID) < Amount)
	{
		return false;
	}

	UWDCountrySubsystem* Countries = GetGameInstance()->GetSubsystem<UWDCountrySubsystem>();
	if (!Countries)
	{
		return false;
	}

	Countries->ModifyStat(FromCountryID, TEXT("Treasury"), -Amount);
	Countries->ModifyStat(ToCountryID, TEXT("Treasury"), Amount);
	return true;
}

double UWDEconomySubsystem::GetTreasury(FName CountryID) const
{
	if (const UWDCountrySubsystem* Countries = GetGameInstance()->GetSubsystem<UWDCountrySubsystem>())
	{
		FWDCountryStats Stats;
		if (Countries->GetCountryStats(CountryID, Stats))
		{
			return Stats.Treasury;
		}
	}
	return 0.0;
}
