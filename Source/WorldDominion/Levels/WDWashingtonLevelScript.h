// Assembles the Phase 1 vertical slice entirely from code: the White House
// exterior placeholder massing, the Chief of Staff / Secretary of State /
// foreign delegation NPCs with their scripted dialogue, the parked motorcade
// dressing, and the Situation Room trigger that surfaces the geopolitical
// crisis - so a single "BP_WashingtonLevelScript -> AWDWashingtonLevelScript"
// (or a native instance) dropped into Washington_DC.umap reproduces the
// opening 15-30 minutes described in the brief with no other manual
// placement required.
//
// Everything this class spawns is a documented placeholder (see class
// comments on AWDGovernmentNPC, AWDPresidentialVehicle, AWDSecurityAgent,
// and PlaceholderBodyMesh throughout) - swap SpawnClass properties for
// art-complete Blueprints once they exist; this script's wiring does not
// change.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WDWashingtonLevelScript.generated.h"

class AWDGovernmentNPC;
class AWDPresidentialVehicle;
class AWDMotorcadeManager;
class AWDMissionTriggerVolume;
class UStaticMeshComponent;

UCLASS()
class WORLDDOMINION_API AWDWashingtonLevelScript : public AActor
{
	GENERATED_BODY()

public:
	AWDWashingtonLevelScript();

	UPROPERTY(EditAnywhere, Category = "WorldDominion|Washington")
	FVector WhiteHouseLocation = FVector(2000.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "WorldDominion|Washington")
	FVector SituationRoomTriggerLocation = FVector(2400.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, Category = "WorldDominion|Washington")
	FVector MotorcadeDressingLocation = FVector(1200.f, 300.f, 0.f);

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Washington")
	TSubclassOf<AWDGovernmentNPC> GovernmentNPCClass;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Washington")
	TSubclassOf<AWDPresidentialVehicle> PresidentialVehicleClass;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Washington")
	FName OpeningMissionID = TEXT("AttendEmergencySummit");

protected:
	virtual void BeginPlay() override;

	void BuildWhiteHousePlaceholder();
	void SpawnStaffNPCs();
	void SpawnMotorcadeDressing();
	void SpawnSituationRoomTrigger();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Placeholder")
	TArray<TObjectPtr<UStaticMeshComponent>> WhiteHouseMassingPieces;
};
