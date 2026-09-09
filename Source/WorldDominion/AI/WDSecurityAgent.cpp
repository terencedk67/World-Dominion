#include "WDSecurityAgent.h"
#include "WorldDominion.h"
#include "UObject/ConstructorHelpers.h"
#include "WDSecurityAgentController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

AWDSecurityAgent::AWDSecurityAgent()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AWDSecurityAgentController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->InitCapsuleSize(38.f, 92.f);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = FollowSpeed;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	PlaceholderBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBodyMesh"));
	PlaceholderBodyMesh->SetupAttachment(RootComponent);
	if (PlaceholderMeshFinder.Succeeded())
	{
		PlaceholderBodyMesh->SetStaticMesh(PlaceholderMeshFinder.Object);
	}
	PlaceholderBodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	PlaceholderBodyMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.7f));
	PlaceholderBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);
}

void AWDSecurityAgent::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = FollowSpeed;
}

void AWDSecurityAgent::SetFollowTarget(AActor* NewTarget)
{
	FollowTarget = NewTarget;
	AgentState = EWDSecurityAgentState::Following;
}

void AWDSecurityAgent::ResumeFollowing()
{
	AgentState = EWDSecurityAgentState::Following;
}

void AWDSecurityAgent::HoldDoorAt(const FVector& WorldLocation, const FRotator& FaceRotation)
{
	AgentState = EWDSecurityAgentState::HoldingDoor;
	HeldDoorLocation = WorldLocation;
	HeldDoorRotation = FaceRotation;
}

void AWDSecurityAgent::NotifyProtectionStateChanged(EWDProtectionState NewState)
{
	switch (NewState)
	{
	case EWDProtectionState::Normal:
		AcceptanceRadius = 60.f;
		break;
	case EWDProtectionState::Heightened:
		AcceptanceRadius = 45.f;
		break;
	case EWDProtectionState::Emergency:
	case EWDProtectionState::Lockdown:
		AcceptanceRadius = 30.f; // tighter escort radius when the threat level is elevated
		break;
	}
}

void AWDSecurityAgent::OnPerceivedActor_Implementation(AActor* PerceivedActor, bool bIsHostile)
{
	if (bIsHostile)
	{
		AgentState = EWDSecurityAgentState::Responding;
		UE_LOG(LogWorldDominion, Warning, TEXT("%s: hostile contact - %s"), *GetName(), *GetNameSafe(PerceivedActor));
	}
}

FVector AWDSecurityAgent::GetDesiredFollowLocation() const
{
	if (const AActor* Target = FollowTarget.Get())
	{
		const FRotator TargetYaw(0.f, Target->GetActorRotation().Yaw, 0.f);
		return Target->GetActorLocation() + TargetYaw.RotateVector(FormationOffset);
	}
	return GetActorLocation();
}

void AWDSecurityAgent::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	switch (AgentState)
	{
	case EWDSecurityAgentState::Following:
	case EWDSecurityAgentState::HoldingPosition:
	{
		const FVector Desired = GetDesiredFollowLocation();
		if (FVector::DistSquared(Desired, GetActorLocation()) > FMath::Square(AcceptanceRadius))
		{
			AIController->MoveToLocation(Desired, AcceptanceRadius * 0.5f, true, true, false, true);
		}
		break;
	}
	case EWDSecurityAgentState::HoldingDoor:
	{
		if (FVector::DistSquared(HeldDoorLocation, GetActorLocation()) > FMath::Square(AcceptanceRadius))
		{
			AIController->MoveToLocation(HeldDoorLocation, AcceptanceRadius * 0.5f);
		}
		SetActorRotation(HeldDoorRotation);
		break;
	}
	case EWDSecurityAgentState::EnteringVehicle:
	case EWDSecurityAgentState::Responding:
	default:
		break;
	}
}
