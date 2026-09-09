#include "WDInfluenceComponent.h"
#include "WDCountrySubsystem.h"
#include "WDCountryStats.h"
#include "WDGameInstance.h"
#include "Kismet/GameplayStatics.h"

FName UWDInfluenceComponent::GetActiveCountryID() const
{
	if (const UWDGameInstance* GI = Cast<UWDGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		return GI->GetSelectedCountry();
	}
	return NAME_None;
}

void UWDInfluenceComponent::AddInfluence(float Amount)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>())
		{
			const float NewValue = Countries->ModifyStat(GetActiveCountryID(), TEXT("DiplomaticInfluence"), Amount);
			OnInfluenceChanged.Broadcast(NewValue, Amount);
		}
	}
}

bool UWDInfluenceComponent::SpendInfluence(float Amount)
{
	if (GetInfluence() < Amount)
	{
		return false;
	}
	AddInfluence(-Amount);
	return true;
}

float UWDInfluenceComponent::GetInfluence() const
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDCountrySubsystem* Countries = GI->GetSubsystem<UWDCountrySubsystem>())
		{
			FWDCountryStats Stats;
			if (Countries->GetCountryStats(GetActiveCountryID(), Stats))
			{
				return Stats.DiplomaticInfluence;
			}
		}
	}
	return 0.f;
}
