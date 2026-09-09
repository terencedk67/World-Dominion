// C++ backing for the Decision popup that presents a world event's branching
// options (see the design brief's "GLOBAL CHIP SHORTAGE" example) inside the
// Situation Room. Listens for UWDWorldEventSubsystem::OnWorldEventTriggered
// and forwards the chosen option back to ResolveEvent - the widget itself
// never touches country stats directly.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDWorldEventData.h"
#include "WDDecisionWidget.generated.h"

UCLASS()
class WORLDDOMINION_API UWDDecisionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|Decision")
	void OnDecisionPresented(const FWDWorldEventDefinition& Event);

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|Decision")
	void OnDecisionResolved(const FText& ResultingHeadline);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Decision")
	void ChooseOption(int32 OptionIndex);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleWorldEventTriggered(FWDWorldEventDefinition Event);

	FName CurrentEventID;
};
