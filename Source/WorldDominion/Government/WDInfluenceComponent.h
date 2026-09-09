// Thin, UI-facing wrapper over the active country's DiplomaticInfluence stat
// (held by UWDCountrySubsystem). Missions/decisions/events grant or spend
// Influence through here so the HUD gets one broadcast delegate to bind to
// instead of polling the subsystem every frame.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WDInfluenceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInfluenceChanged, float, NewValue, float, Delta);

UCLASS(ClassGroup = (WorldDominion), meta = (BlueprintSpawnableComponent))
class WORLDDOMINION_API UWDInfluenceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "WorldDominion|Influence")
	FOnInfluenceChanged OnInfluenceChanged;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Influence")
	void AddInfluence(float Amount);

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Influence")
	bool SpendInfluence(float Amount);

	UFUNCTION(BlueprintPure, Category = "WorldDominion|Influence")
	float GetInfluence() const;

protected:
	FName GetActiveCountryID() const;
};
