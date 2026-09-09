// Attached to AWDPlayerCharacter. Sphere/line-traces forward each tick for the
// closest IWDInteractable, exposes it to the HUD for the on-screen prompt, and
// triggers OnInteract on input. Deliberately dumb/generic - all "what happens"
// logic lives on the interactable actor, never here.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WDInteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFocusedInteractableChanged, AActor*, NewFocus);

UCLASS(ClassGroup = (WorldDominion), meta = (BlueprintSpawnableComponent))
class WORLDDOMINION_API UWDInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Interaction")
	float TraceDistance = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Interaction")
	float TraceRadius = 40.f;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Interaction")
	FOnFocusedInteractableChanged OnFocusedInteractableChanged;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Interaction")
	void TryInteract();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Interaction")
	AActor* GetFocusedInteractable() const { return FocusedInteractable.Get(); }

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> FocusedInteractable;
};
