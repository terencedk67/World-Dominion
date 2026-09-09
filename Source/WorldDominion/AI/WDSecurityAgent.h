// A Secret Service / security detail member. Follows the protectee in a
// formation slot assigned by UWDPresidentialSecurityComponent, escalates
// alertness with the protection state, and can be sent to hold a door or
// clear a path. Uses a small native state machine rather than a Behavior
// Tree asset so the vertical slice runs with zero .uasset BT/Blackboard
// authoring; swap FollowState/Tick logic for a real Behavior Tree or
// StateTree asset once content tools are available in-editor.
//
// PLACEHOLDER: dark cylinder stand-in body (see PlaceholderBodyMesh).
// FINAL ASSET REQUIRED: suited Secret Service agent Skeletal Mesh + anims,
// earpiece/comms prop, sidearm holster prop.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WDTypes.h"
#include "WDSecurityAgent.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EWDSecurityAgentState : uint8
{
	Following,
	HoldingPosition,
	HoldingDoor,
	EnteringVehicle,
	Responding
};

UCLASS()
class WORLDDOMINION_API AWDSecurityAgent : public ACharacter
{
	GENERATED_BODY()

public:
	AWDSecurityAgent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Placeholder")
	TObjectPtr<UStaticMeshComponent> PlaceholderBodyMesh;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void SetFollowTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void SetFormationOffset(const FVector& NewOffset) { FormationOffset = NewOffset; }

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void HoldDoorAt(const FVector& WorldLocation, const FRotator& FaceRotation);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void ResumeFollowing();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void NotifyProtectionStateChanged(EWDProtectionState NewState);

	UFUNCTION(BlueprintPure, Category = "WorldDominion|Security")
	EWDSecurityAgentState GetAgentState() const { return AgentState; }

	/** Called by the agent's AIPerceptionComponent (see AWDSecurityAgentController) when a
	 *  new actor enters sight. Phase 1 stub: logs + broadcasts for a future threat-response
	 *  system (raising the detail's UWDPresidentialSecurityComponent protection state). */
	UFUNCTION(BlueprintNativeEvent, Category = "WorldDominion|Security")
	void OnPerceivedActor(AActor* PerceivedActor, bool bIsHostile);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Security")
	float FollowSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Security")
	float AcceptanceRadius = 60.f;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> FollowTarget;

	UPROPERTY(Transient)
	FVector FormationOffset = FVector::ZeroVector;

	EWDSecurityAgentState AgentState = EWDSecurityAgentState::Following;

	FVector HeldDoorLocation = FVector::ZeroVector;
	FRotator HeldDoorRotation = FRotator::ZeroRotator;

	FVector GetDesiredFollowLocation() const;
};
