// Gathers/restores world state to/from a UWDSaveGame slot. Talks only to
// other subsystems' public APIs (UWDCountrySubsystem, UWDMissionSubsystem,
// UWDGameInstance) so it never needs to know the internal shape of any one
// system's data.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDSaveSubsystem.generated.h"

class UWDSaveGame;

UCLASS()
class WORLDDOMINION_API UWDSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Save")
	bool SaveWorldState(const FString& SlotName = TEXT(""));

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Save")
	bool LoadWorldState(const FString& SlotName = TEXT(""));

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Save")
	bool DoesSaveExist(const FString& SlotName = TEXT("")) const;
};
