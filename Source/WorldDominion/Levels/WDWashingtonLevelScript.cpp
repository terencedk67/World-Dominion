#include "WDWashingtonLevelScript.h"
#include "WorldDominion.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "WDGovernmentNPC.h"
#include "WDPresidentialVehicle.h"
#include "WDMotorcadeManager.h"
#include "WDMissionTriggerVolume.h"
#include "WDMissionSubsystem.h"
#include "Kismet/GameplayStatics.h"

AWDWashingtonLevelScript::AWDWashingtonLevelScript()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("Root")));
}

void AWDWashingtonLevelScript::BeginPlay()
{
	Super::BeginPlay();

	BuildWhiteHousePlaceholder();
	SpawnStaffNPCs();
	SpawnMotorcadeDressing();
	SpawnSituationRoomTrigger();

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
		{
			Missions->StartMission(OpeningMissionID);
		}
	}
}

namespace
{
	UStaticMeshComponent* MakeMassingPiece(AActor* Owner, USceneComponent* AttachParent, const TCHAR* MeshPath, FName Name, const FVector& RelativeLocation, const FVector& RelativeScale)
	{
		UStaticMeshComponent* Comp = NewObject<UStaticMeshComponent>(Owner, Name);
		Comp->SetupAttachment(AttachParent);
		if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, MeshPath))
		{
			Comp->SetStaticMesh(Mesh);
		}
		Comp->SetRelativeLocation(RelativeLocation);
		Comp->SetRelativeScale3D(RelativeScale);
		Comp->SetMobility(EComponentMobility::Static);
		Comp->RegisterComponent();
		return Comp;
	}
}

void AWDWashingtonLevelScript::BuildWhiteHousePlaceholder()
{
	// PLACEHOLDER: White House exterior built from engine basic shapes (a main block,
	// a portico, and a colonnade). FINAL ASSET REQUIRED: authored White House exterior
	// (or licensed-alike government-mansion) environment art, World Partition streamed.
	const TCHAR* CubePath = TEXT("/Engine/BasicShapes/Cube.Cube");
	const TCHAR* CylinderPath = TEXT("/Engine/BasicShapes/Cylinder.Cylinder");

	WhiteHouseMassingPieces.Add(MakeMassingPiece(this, RootComponent, CubePath, TEXT("WH_MainBlock"),
		WhiteHouseLocation + FVector(0.f, 0.f, 300.f), FVector(12.f, 8.f, 6.f)));

	WhiteHouseMassingPieces.Add(MakeMassingPiece(this, RootComponent, CubePath, TEXT("WH_Portico"),
		WhiteHouseLocation + FVector(-650.f, 0.f, 220.f), FVector(2.f, 4.f, 4.4f)));

	for (int32 Index = 0; Index < 4; ++Index)
	{
		const float YOffset = -240.f + Index * 160.f;
		WhiteHouseMassingPieces.Add(MakeMassingPiece(this, RootComponent, CylinderPath,
			*FString::Printf(TEXT("WH_Column%d"), Index),
			WhiteHouseLocation + FVector(-700.f, YOffset, 220.f), FVector(0.5f, 0.5f, 4.4f)));
	}
}

void AWDWashingtonLevelScript::SpawnStaffNPCs()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const TSubclassOf<AWDGovernmentNPC> NPCClass = GovernmentNPCClass ? GovernmentNPCClass : AWDGovernmentNPC::StaticClass();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossible;

	if (AWDGovernmentNPC* ChiefOfStaff = World->SpawnActor<AWDGovernmentNPC>(NPCClass,
		WhiteHouseLocation + FVector(-500.f, 150.f, 0.f), FRotator::ZeroRotator, SpawnParams))
	{
		ChiefOfStaff->Profile.DisplayName = NSLOCTEXT("WorldDominion", "ChiefOfStaffName", "Chief of Staff Miller");
		ChiefOfStaff->Profile.RoleTitle = NSLOCTEXT("WorldDominion", "ChiefOfStaffRole", "Chief of Staff");
		ChiefOfStaff->DialogueLines = {
			NSLOCTEXT("WorldDominion", "COSLine1", "Mr. President, the National Security Council is waiting."),
			NSLOCTEXT("WorldDominion", "COSLine2", "They're in the Situation Room whenever you're ready.")
		};
		ChiefOfStaff->SatisfiesObjectiveID = TEXT("TalkToChiefOfStaff");
	}

	if (AWDGovernmentNPC* SecState = World->SpawnActor<AWDGovernmentNPC>(NPCClass,
		WhiteHouseLocation + FVector(-500.f, -150.f, 0.f), FRotator::ZeroRotator, SpawnParams))
	{
		SecState->Profile.DisplayName = NSLOCTEXT("WorldDominion", "SecStateName", "Secretary Whitfield");
		SecState->Profile.RoleTitle = NSLOCTEXT("WorldDominion", "SecStateRole", "Secretary of State");
		SecState->DialogueLines = {
			NSLOCTEXT("WorldDominion", "SecStateLine1", "Good morning, Mr. President. Our allies are watching how we handle this closely.")
		};
		SecState->SatisfiesObjectiveID = TEXT("TalkToSecretaryOfState");
	}

	if (AWDGovernmentNPC* Delegation = World->SpawnActor<AWDGovernmentNPC>(NPCClass,
		WhiteHouseLocation + FVector(-350.f, 0.f, 0.f), FRotator::ZeroRotator, SpawnParams))
	{
		Delegation->Profile.DisplayName = NSLOCTEXT("WorldDominion", "DelegationName", "Ambassador Laurent");
		Delegation->Profile.RoleTitle = NSLOCTEXT("WorldDominion", "DelegationRole", "Foreign Delegation");
		Delegation->DialogueLines = {
			NSLOCTEXT("WorldDominion", "DelegationLine1", "Mr. President, thank you for making time for us this morning.")
		};
		Delegation->SatisfiesObjectiveID = TEXT("MeetForeignDelegation");
	}
}

void AWDWashingtonLevelScript::SpawnMotorcadeDressing()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const TSubclassOf<AWDPresidentialVehicle> VehicleClass = PresidentialVehicleClass ? PresidentialVehicleClass : AWDPresidentialVehicle::StaticClass();
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossible;

	AWDPresidentialVehicle* Limousine = World->SpawnActor<AWDPresidentialVehicle>(VehicleClass,
		MotorcadeDressingLocation, FRotator(0.f, 180.f, 0.f), SpawnParams);

	TArray<AWDPresidentialVehicle*> Escorts;
	for (int32 Index = 0; Index < 2; ++Index)
	{
		const FVector Offset = FVector(Index == 0 ? 500.f : -500.f, 0.f, 0.f);
		if (AWDPresidentialVehicle* Escort = World->SpawnActor<AWDPresidentialVehicle>(VehicleClass,
			MotorcadeDressingLocation + Offset, FRotator(0.f, 180.f, 0.f), SpawnParams))
		{
			Escorts.Add(Escort);
		}
	}

	if (Limousine)
	{
		if (AWDMotorcadeManager* Motorcade = World->SpawnActor<AWDMotorcadeManager>(AWDMotorcadeManager::StaticClass()))
		{
			Motorcade->LeadVehicle = Limousine;
			Motorcade->EscortVehicles = Escorts;
			// Parked dressing for the opening scene per the reference art; BeginDeparture/
			// BeginArrival are ready to be called from a later mission (e.g. departing for
			// Andrews) once that route's waypoints are authored in-editor for this level.
		}
	}
}

void AWDWashingtonLevelScript::SpawnSituationRoomTrigger()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (AWDMissionTriggerVolume* Trigger = World->SpawnActor<AWDMissionTriggerVolume>(
		AWDMissionTriggerVolume::StaticClass(), SituationRoomTriggerLocation, FRotator::ZeroRotator))
	{
		Trigger->ObjectiveIDs = { TEXT("EnterSituationRoom"), TEXT("AttendNSCMeeting") };
		Trigger->EventIDToTrigger = TEXT("GlobalChipShortage");
		Trigger->bTriggerOnce = true;
	}
}
