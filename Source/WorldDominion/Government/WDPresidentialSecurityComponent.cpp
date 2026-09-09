#include "WDPresidentialSecurityComponent.h"
#include "WorldDominion.h"
#include "WDSecurityAgent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"

namespace
{
	// Loose diamond formation around the protectee: two lead flanks slightly ahead,
	// two trail flanks slightly behind, extra agents ring further out as the detail grows.
	FVector GetFormationOffsetForIndex(int32 Index, int32 Total)
	{
		static const TArray<FVector> BaseSlots = {
			FVector(150.f,  120.f, 0.f),	// front-right flank
			FVector(150.f, -120.f, 0.f),	// front-left flank
			FVector(-180.f,  140.f, 0.f),	// rear-right flank
			FVector(-180.f, -140.f, 0.f),	// rear-left flank
			FVector(260.f,   0.f, 0.f),		// point
			FVector(-320.f,  0.f, 0.f),		// tail
			FVector(0.f,   260.f, 0.f),		// right wide
			FVector(0.f,  -260.f, 0.f)		// left wide
		};
		return BaseSlots.IsValidIndex(Index) ? BaseSlots[Index] : FVector(-200.f - 60.f * Index, 0.f, 0.f);
	}
}

UWDPresidentialSecurityComponent::UWDPresidentialSecurityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	DetailSizeByState.Add(EWDProtectionState::Normal, 2);
	DetailSizeByState.Add(EWDProtectionState::Heightened, 4);
	DetailSizeByState.Add(EWDProtectionState::Emergency, 6);
	DetailSizeByState.Add(EWDProtectionState::Lockdown, 8);
}

void UWDPresidentialSecurityComponent::BeginPlay()
{
	Super::BeginPlay();
	EnsureDetailSize();
}

void UWDPresidentialSecurityComponent::SetProtectionState(EWDProtectionState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
	EnsureDetailSize();
	ReformFormation();

	for (AWDSecurityAgent* Agent : Detail)
	{
		if (Agent)
		{
			Agent->NotifyProtectionStateChanged(CurrentState);
		}
	}

	OnProtectionStateChanged.Broadcast(CurrentState);
}

void UWDPresidentialSecurityComponent::OnProtecteePossessed(APawn* NewProtectee)
{
	Protectee = NewProtectee;
	EnsureDetailSize();
	ReformFormation();

	for (AWDSecurityAgent* Agent : Detail)
	{
		if (Agent)
		{
			Agent->SetFollowTarget(NewProtectee);
		}
	}
}

AWDSecurityAgent* UWDPresidentialSecurityComponent::SpawnAgent() const
{
	if (!SecurityAgentClass)
	{
		UE_LOG(LogWorldDominion, Warning, TEXT("UWDPresidentialSecurityComponent has no SecurityAgentClass assigned."));
		return nullptr;
	}

	APawn* ProtecteePawn = Protectee.Get();
	if (!ProtecteePawn)
	{
		return nullptr;
	}

	UWorld* World = ProtecteePawn->GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossible;
	return World->SpawnActor<AWDSecurityAgent>(SecurityAgentClass, ProtecteePawn->GetActorLocation(), ProtecteePawn->GetActorRotation(), SpawnParams);
}

void UWDPresidentialSecurityComponent::EnsureDetailSize()
{
	const int32* DesiredPtr = DetailSizeByState.Find(CurrentState);
	const int32 Desired = DesiredPtr ? *DesiredPtr : 2;

	// Drop stale (destroyed) references before comparing counts.
	Detail.RemoveAll([](const TObjectPtr<AWDSecurityAgent>& A) { return !IsValid(A); });

	while (Detail.Num() < Desired)
	{
		if (AWDSecurityAgent* NewAgent = SpawnAgent())
		{
			Detail.Add(NewAgent);
		}
		else
		{
			break;
		}
	}

	while (Detail.Num() > Desired)
	{
		AWDSecurityAgent* Extra = Detail.Pop();
		if (IsValid(Extra))
		{
			Extra->Destroy();
		}
	}
}

void UWDPresidentialSecurityComponent::ReformFormation()
{
	for (int32 Index = 0; Index < Detail.Num(); ++Index)
	{
		if (AWDSecurityAgent* Agent = Detail[Index])
		{
			Agent->SetFormationOffset(GetFormationOffsetForIndex(Index, Detail.Num()));
		}
	}
}
