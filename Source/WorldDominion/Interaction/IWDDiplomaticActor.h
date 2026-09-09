// Implemented by foreign leaders / ambassadors / delegation NPCs the player can
// open diplomatic actions against (CallLeader, RequestSummit, negotiate, ...).
// Keeps UWDDiplomacySubsystem decoupled from any concrete NPC Blueprint class.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WDTypes.h"
#include "IWDDiplomaticActor.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UWDDiplomaticActor : public UInterface
{
	GENERATED_BODY()
};

class WORLDDOMINION_API IWDDiplomaticActor
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Diplomacy")
	FName GetRepresentedCountryID() const;

	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Diplomacy")
	bool SupportsDiplomaticAction(EWDDiplomaticAction Action) const;

	/** Opens the in-world meeting flow (player walks in, sits, cinematic dialogue begins). */
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Diplomacy")
	void BeginMeeting(AActor* Instigator);
};
