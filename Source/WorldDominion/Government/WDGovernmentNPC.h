// Base class for Chief of Staff, Secretary of State, Defense Minister, press
// secretaries, ambassadors, etc. Deterministic dialogue for Phase 1 (a fixed
// line list played in order) behind a small interface so a later pass can
// swap in LLM-driven responses without touching callers - anything that
// walks up and interacts, or any mission, only ever sees
// IWDInteractable / IWDMissionTarget / IWDDiplomaticActor.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "IWDInteractable.h"
#include "IWDMissionTarget.h"
#include "WDGovernmentNPC.generated.h"

class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FWDNPCProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FText RoleTitle;

	/** Country this NPC represents/serves, resolved against UWDCountrySubsystem. NAME_None = player's nation staff. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	FName CountryID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	int32 Trust = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC")
	int32 Influence = 50;
};

UCLASS()
class WORLDDOMINION_API AWDGovernmentNPC : public ACharacter, public IWDInteractable, public IWDMissionTarget
{
	GENERATED_BODY()

public:
	AWDGovernmentNPC();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|NPC")
	FWDNPCProfile Profile;

	/** Deterministic dialogue framework: played in order each time this NPC is interacted with,
	 *  clamped to the last line once exhausted. Ready to be swapped for an LLM-backed line
	 *  generator later without changing OnInteract's contract with callers. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Dialogue")
	TArray<FText> DialogueLines;

	/** Objective ID (matches an FWDMissionObjective::ObjectiveID) completed by talking to this NPC. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WorldDominion|Mission")
	FName SatisfiesObjectiveID = NAME_None;

	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|Dialogue")
	void OnDialogueLine(const FText& Speaker, const FText& Line);

	// IWDInteractable
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual FGameplayTagContainer GetInteractionTags_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Instigator) const override;
	virtual void OnInteract_Implementation(AActor* Instigator) override;

	// IWDMissionTarget
	virtual FName GetObjectiveID_Implementation() const override { return SatisfiesObjectiveID; }
	virtual void NotifyObjectiveSatisfied_Implementation(AActor* Instigator) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Placeholder")
	TObjectPtr<UStaticMeshComponent> PlaceholderBodyMesh;

	int32 NextDialogueIndex = 0;
};
