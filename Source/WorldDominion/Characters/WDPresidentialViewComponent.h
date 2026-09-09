// PRESIDENTIAL VIEW - a sophisticated intelligence/awareness overlay, not a
// magic "detective vision". While active it:
//   - queries nearby actors implementing IWDInteractable / IWDMissionTarget /
//     IWDDiplomaticActor and tags relevant ones with a category (person,
//     security, objective, vehicle, diplomatic, intel)
//   - applies a per-category custom-depth stencil value so a post-process
//     material (authored in-editor) can render a restrained outline/glow -
//     never a screen-wide color wash
//   - drives a short in/out transition and a subtle desaturation of the base
//     scene so the overlay reads as "enhanced awareness", not a superpower
//
// PLACEHOLDER: outline driven by engine custom-depth + a stock post-process
// material. FINAL ASSET REQUIRED: a bespoke Presidential View post-process
// material/material function matching the gold/navy HUD language.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "WDPresidentialViewComponent.generated.h"

UENUM(BlueprintType)
enum class EWDPresidentialViewCategory : uint8
{
	None,
	Person,
	Security,
	Objective,
	Vehicle,
	Diplomatic,
	Intel
};

/** Custom depth stencil values consumed by the Presidential View post-process material.
 *  Kept as a small fixed enum->int table rather than magic numbers scattered in code. */
UCLASS()
class WORLDDOMINION_API UWDPresidentialViewStencils : public UObject
{
	GENERATED_BODY()
public:
	static int32 GetStencilValue(EWDPresidentialViewCategory Category);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPresidentialViewToggled, bool, bIsActive);

UCLASS(ClassGroup = (WorldDominion), meta = (BlueprintSpawnableComponent))
class WORLDDOMINION_API UWDPresidentialViewComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDPresidentialViewComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|PresidentialView")
	float ScanRadius = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|PresidentialView")
	float RescanInterval = 0.35f;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|PresidentialView")
	FOnPresidentialViewToggled OnToggled;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|PresidentialView")
	void ToggleActive();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|PresidentialView")
	void SetActive(bool bNewActive);

	UFUNCTION(BlueprintPure, Category = "WorldDominion|PresidentialView")
	bool IsActive() const { return bActive; }

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RescanNearbyActors();
	EWDPresidentialViewCategory ClassifyActor(AActor* Actor) const;

	bool bActive = false;
	float TimeSinceLastScan = 0.f;

	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<AActor>> HighlightedActors;
};
