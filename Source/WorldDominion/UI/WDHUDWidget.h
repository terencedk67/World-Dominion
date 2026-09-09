// C++ backing logic for the gameplay HUD (top-right leader/influence/treasury/
// allies/approval, top-left objectives, bottom-left minimap, bottom-right
// contextual prompt - see the Washington gameplay reference art). This class
// wires subsystem delegates to a small set of BlueprintImplementableEvents;
// the actual premium UMG layout (gold hairlines, translucent panels, the
// serif/sans type pairing) is authored in a WBP_HUD Blueprint child in-editor
// and simply implements these events to drive real widgets.
//
// PLACEHOLDER: none - this class has no visuals of its own until a UMG
// Blueprint child overrides the implementable events below.
// FINAL ASSET REQUIRED: WBP_HUD UMG layout matching the key art HUD language.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDMissionData.h"
#include "WDHUDWidget.generated.h"

UCLASS()
class WORLDDOMINION_API UWDHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// --- Top-right: leader / influence / treasury / allies / approval ---
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|HUD")
	void OnTopLineStatsUpdated(float Influence, float Treasury, int32 AllyCount, float PublicApproval);

	// --- Top-left: main + secondary objectives ---
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|HUD")
	void OnObjectivesUpdated(const TArray<FWDMissionObjective>& ActiveObjectives);

	// --- Bottom-right: contextual interaction prompt ---
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|HUD")
	void OnInteractionPromptUpdated(const FText& Prompt, bool bVisible);

	// --- Presidential View toggle feedback (edge vignette / awareness-mode indicator) ---
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|HUD")
	void OnPresidentialViewToggled(bool bIsActive);

	// --- Media ticker (see WDDiplomacySubsystem::OnDiplomaticHeadline / WorldEvent resolution) ---
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|HUD")
	void OnHeadline(const FText& Headline);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleTopLineStatsChanged();

	UFUNCTION()
	void HandleObjectiveStateChanged(FName ObjectiveID, EWDObjectiveState NewState);

	UFUNCTION()
	void HandleFocusedInteractableChanged(AActor* NewFocus);

	UFUNCTION()
	void HandlePresidentialViewToggled(bool bIsActive);

	UFUNCTION()
	void HandleDiplomaticHeadline(FText Headline);

	void RefreshObjectives();
};
