// The entourage system. Lives on AWDPlayerController (survives pawn possession
// changes across on-foot / vehicle / aircraft) and owns a roster of
// AWDSecurityAgent detail members who follow the protectee in formation, scan
// for threats, clear doors, and enter/exit vehicles alongside the President.
// This is what makes the player feel like a head of state rather than a lone
// wanderer - see NORMAL/HEIGHTENED/EMERGENCY/LOCKDOWN in EWDProtectionState.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WDTypes.h"
#include "WDPresidentialSecurityComponent.generated.h"

class AWDSecurityAgent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnProtectionStateChanged, EWDProtectionState, NewState);

UCLASS(ClassGroup = (WorldDominion), meta = (BlueprintSpawnableComponent))
class WORLDDOMINION_API UWDPresidentialSecurityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWDPresidentialSecurityComponent();

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Security")
	TSubclassOf<AWDSecurityAgent> SecurityAgentClass;

	/** Detail size per protection state - Emergency/Lockdown fold in additional agents. */
	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Security")
	TMap<EWDProtectionState, int32> DetailSizeByState;

	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Security")
	FOnProtectionStateChanged OnProtectionStateChanged;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void SetProtectionState(EWDProtectionState NewState);

	UFUNCTION(BlueprintPure, Category = "WorldDominion|Security")
	EWDProtectionState GetProtectionState() const { return CurrentState; }

	/** Called by AWDPlayerController::OnPossess so the detail retargets to whatever the
	 *  President currently occupies (on foot, or a specific vehicle seat cluster). */
	void OnProtecteePossessed(APawn* NewProtectee);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void EnsureDetailSize();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	const TArray<AWDSecurityAgent*>& GetDetail() const { return Detail; }

	/** Recomputes each agent's formation slot offset around the protectee. Call after the
	 *  detail size or protection state changes; agents re-path to their new slot themselves. */
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Security")
	void ReformFormation();

protected:
	virtual void BeginPlay() override;

	EWDProtectionState CurrentState = EWDProtectionState::Normal;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AWDSecurityAgent>> Detail;

	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> Protectee;

	AWDSecurityAgent* SpawnAgent() const;
};
