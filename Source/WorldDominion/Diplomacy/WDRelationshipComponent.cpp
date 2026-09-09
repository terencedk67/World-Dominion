#include "WDRelationshipComponent.h"
#include "WDDiplomacySubsystem.h"
#include "WDGameInstance.h"
#include "Kismet/GameplayStatics.h"

FWDRelationshipState UWDRelationshipComponent::GetRelationshipWithPlayerCountry() const
{
	if (const UWDGameInstance* GI = Cast<UWDGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (UWDDiplomacySubsystem* Diplomacy = GI->GetSubsystem<UWDDiplomacySubsystem>())
		{
			return Diplomacy->GetRelationship(GI->GetSelectedCountry(), RepresentedCountryID);
		}
	}
	return FWDRelationshipState();
}

float UWDRelationshipComponent::ModifyRelationshipWithPlayerCountry(FName Dimension, float Delta)
{
	if (UWDGameInstance* GI = Cast<UWDGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (UWDDiplomacySubsystem* Diplomacy = GI->GetSubsystem<UWDDiplomacySubsystem>())
		{
			return Diplomacy->ModifyRelationship(GI->GetSelectedCountry(), RepresentedCountryID, Dimension, Delta);
		}
	}
	return 0.f;
}
