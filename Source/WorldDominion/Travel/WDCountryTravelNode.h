// A physical departure/arrival point in the world (Andrews-style air base
// tarmac, a capital's motorcade drop-off, an airport gate). The player
// interacts with it to open the Travel app / kick off UWDTravelSubsystem.
// Placed per-level; carries no per-country branching logic itself.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IWDInteractable.h"
#include "WDCountryTravelNode.generated.h"

UCLASS()
class WORLDDOMINION_API AWDCountryTravelNode : public AActor, public IWDInteractable
{
	GENERATED_BODY()

public:
	AWDCountryTravelNode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Travel")
	FName DestinationCountryID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Travel")
	FText DestinationDisplayName;

	// IWDInteractable
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual FGameplayTagContainer GetInteractionTags_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Instigator) const override { return true; }
	virtual void OnInteract_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Placeholder")
	TObjectPtr<class UStaticMeshComponent> PlaceholderMarkerMesh;
};
