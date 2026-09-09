#include "WDPresidentialVehicle.h"
#include "WorldDominion.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"

AWDPresidentialVehicle::AWDPresidentialVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	PlaceholderBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBodyMesh"));
	PlaceholderBodyMesh->SetupAttachment(RootComponent);
	if (PlaceholderMeshFinder.Succeeded())
	{
		PlaceholderBodyMesh->SetStaticMesh(PlaceholderMeshFinder.Object);
	}
	// Scaled cube: a Cadillac-One-style limousine silhouette placeholder.
	PlaceholderBodyMesh->SetRelativeScale3D(FVector(3.0f, 1.1f, 0.65f));
	PlaceholderBodyMesh->SetRelativeLocation(FVector(0.f, 0.f, 42.f));
	PlaceholderBodyMesh->SetCollisionProfileName(TEXT("Vehicle"));

	DriverSeat = CreateDefaultSubobject<USceneComponent>(TEXT("DriverSeat"));
	DriverSeat->SetupAttachment(RootComponent);
	DriverSeat->SetRelativeLocation(FVector(120.f, -45.f, 60.f));

	ProtecteeSeat = CreateDefaultSubobject<USceneComponent>(TEXT("ProtecteeSeat"));
	ProtecteeSeat->SetupAttachment(RootComponent);
	ProtecteeSeat->SetRelativeLocation(FVector(-100.f, 0.f, 60.f));

	for (int32 Index = 0; Index < 2; ++Index)
	{
		USceneComponent* SecuritySeat = CreateDefaultSubobject<USceneComponent>(*FString::Printf(TEXT("SecuritySeat%d"), Index));
		SecuritySeat->SetupAttachment(RootComponent);
		SecuritySeat->SetRelativeLocation(FVector(Index == 0 ? 120.f : -100.f, 45.f, 60.f));
		SecuritySeats.Add(SecuritySeat);
	}
}

USceneComponent* AWDPresidentialVehicle::GetSeatComponent(EWDVehicleSeatType SeatType, int32 Index) const
{
	switch (SeatType)
	{
	case EWDVehicleSeatType::Driver:		return DriverSeat;
	case EWDVehicleSeatType::Protectee:		return ProtecteeSeat;
	case EWDVehicleSeatType::SecurityDetail:	return SecuritySeats.IsValidIndex(Index) ? SecuritySeats[Index] : nullptr;
	case EWDVehicleSeatType::Passenger:		return ProtecteeSeat; // Phase 1: passengers share the protectee bench.
	default:								return nullptr;
	}
}

bool AWDPresidentialVehicle::HasFreeSeat_Implementation(EWDVehicleSeatType SeatType) const
{
	if (SeatType == EWDVehicleSeatType::SecurityDetail)
	{
		for (USceneComponent* Seat : SecuritySeats)
		{
			if (!SeatOccupants.Contains(Seat) || !SeatOccupants[Seat].IsValid())
			{
				return true;
			}
		}
		return false;
	}

	USceneComponent* Seat = GetSeatComponent(SeatType);
	return Seat && (!SeatOccupants.Contains(Seat) || !SeatOccupants[Seat].IsValid());
}

bool AWDPresidentialVehicle::EnterVehicle_Implementation(APawn* RequestingPawn, EWDVehicleSeatType SeatType)
{
	if (!RequestingPawn)
	{
		return false;
	}

	USceneComponent* Seat = nullptr;
	if (SeatType == EWDVehicleSeatType::SecurityDetail)
	{
		for (USceneComponent* Candidate : SecuritySeats)
		{
			if (!SeatOccupants.Contains(Candidate) || !SeatOccupants[Candidate].IsValid())
			{
				Seat = Candidate;
				break;
			}
		}
	}
	else
	{
		Seat = GetSeatComponent(SeatType);
	}

	if (!Seat || (SeatOccupants.Contains(Seat) && SeatOccupants[Seat].IsValid()))
	{
		return false;
	}

	RequestingPawn->SetActorLocation(Seat->GetComponentLocation());
	RequestingPawn->SetActorRotation(Seat->GetComponentRotation());
	RequestingPawn->AttachToComponent(Seat, FAttachmentTransformRules::SnapToTargetIncludingScale);
	RequestingPawn->SetActorHiddenInGame(true);
	RequestingPawn->SetActorEnableCollision(false);

	SeatOccupants.Add(Seat, RequestingPawn);
	return true;
}

bool AWDPresidentialVehicle::ExitVehicle_Implementation(APawn* RequestingPawn)
{
	if (!RequestingPawn)
	{
		return false;
	}

	for (auto& Pair : SeatOccupants)
	{
		if (Pair.Value.Get() == RequestingPawn)
		{
			RequestingPawn->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			const FVector ExitOffset = GetActorRightVector() * 150.f;
			RequestingPawn->SetActorLocation(GetActorLocation() + ExitOffset);
			RequestingPawn->SetActorHiddenInGame(false);
			RequestingPawn->SetActorEnableCollision(true);
			Pair.Value.Reset();
			return true;
		}
	}
	return false;
}

void AWDPresidentialVehicle::MoveAlongWaypoints(const TArray<FVector>& Waypoints)
{
	ActiveWaypoints = Waypoints;
	CurrentWaypointIndex = ActiveWaypoints.Num() > 0 ? 0 : INDEX_NONE;
}

void AWDPresidentialVehicle::StopMoving()
{
	ActiveWaypoints.Reset();
	CurrentWaypointIndex = INDEX_NONE;
}

void AWDPresidentialVehicle::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!ActiveWaypoints.IsValidIndex(CurrentWaypointIndex))
	{
		return;
	}

	const FVector Target = ActiveWaypoints[CurrentWaypointIndex];
	const FVector ToTarget = Target - GetActorLocation();
	const float DistanceThisFrame = CruiseSpeed * DeltaSeconds;

	if (ToTarget.SizeSquared() <= FMath::Square(DistanceThisFrame))
	{
		SetActorLocation(Target);
		++CurrentWaypointIndex;
		if (!ActiveWaypoints.IsValidIndex(CurrentWaypointIndex))
		{
			CurrentWaypointIndex = INDEX_NONE;
			OnVehicleArrived.Broadcast();
		}
		return;
	}

	const FVector Direction = ToTarget.GetSafeNormal();
	SetActorLocation(GetActorLocation() + Direction * DistanceThisFrame);

	const FRotator DesiredRotation = Direction.Rotation();
	const FRotator NewRotation = FMath::RInterpConstantTo(GetActorRotation(), DesiredRotation, DeltaSeconds, TurnSpeedDegPerSec);
	SetActorRotation(NewRotation);
}
