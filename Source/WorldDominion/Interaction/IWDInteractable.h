// Implemented by anything the player's UWDInteractionComponent can focus and
// trigger: doors, staff NPCs, terminals, phones, vehicles (in addition to
// IWDVehicleInteractable), briefing documents, etc.
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "IWDInteractable.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UWDInteractable : public UInterface
{
	GENERATED_BODY()
};

class WORLDDOMINION_API IWDInteractable
{
	GENERATED_BODY()

public:
	/** Short verb prompt, e.g. "Talk", "Enter", "Review". Drives the on-screen prompt text/icon. */
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Interaction")
	FText GetInteractionPrompt() const;

	/** Which Interact.* / PresidentialView.* tags describe this object, for prompt icon + PV highlight color. */
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Interaction")
	FGameplayTagContainer GetInteractionTags() const;

	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Interaction")
	bool CanInteract(AActor* Instigator) const;

	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Interaction")
	void OnInteract(AActor* Instigator);
};
