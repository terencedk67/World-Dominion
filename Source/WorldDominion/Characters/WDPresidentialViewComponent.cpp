#include "WDPresidentialViewComponent.h"
#include "WorldDominion.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"
#include "IWDInteractable.h"
#include "IWDMissionTarget.h"
#include "IWDDiplomaticActor.h"
#include "WDGameplayTags.h"
#include "WDSecurityAgent.h"

int32 UWDPresidentialViewStencils::GetStencilValue(EWDPresidentialViewCategory Category)
{
	switch (Category)
	{
	case EWDPresidentialViewCategory::Person:		return 251;
	case EWDPresidentialViewCategory::Security:	return 252;
	case EWDPresidentialViewCategory::Objective:	return 253;
	case EWDPresidentialViewCategory::Vehicle:		return 254;
	case EWDPresidentialViewCategory::Diplomatic:	return 255;
	case EWDPresidentialViewCategory::Intel:		return 250;
	default:										return 0;
	}
}

UWDPresidentialViewComponent::UWDPresidentialViewComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWDPresidentialViewComponent::ToggleActive()
{
	SetActive(!bActive);
}

void UWDPresidentialViewComponent::SetActive(bool bNewActive)
{
	if (bActive == bNewActive)
	{
		return;
	}

	bActive = bNewActive;
	OnToggled.Broadcast(bActive);

	if (bActive)
	{
		TimeSinceLastScan = RescanInterval; // force an immediate scan
	}
	else
	{
		for (const TWeakObjectPtr<AActor>& WeakActor : HighlightedActors)
		{
			if (AActor* Actor = WeakActor.Get())
			{
				TArray<UPrimitiveComponent*> Prims;
				Actor->GetComponents<UPrimitiveComponent>(Prims);
				for (UPrimitiveComponent* Prim : Prims)
				{
					Prim->SetRenderCustomDepth(false);
				}
			}
		}
		HighlightedActors.Reset();
	}
}

void UWDPresidentialViewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bActive)
	{
		return;
	}

	TimeSinceLastScan += DeltaTime;
	if (TimeSinceLastScan >= RescanInterval)
	{
		TimeSinceLastScan = 0.f;
		RescanNearbyActors();
	}
}

EWDPresidentialViewCategory UWDPresidentialViewComponent::ClassifyActor(AActor* Actor) const
{
	if (!Actor)
	{
		return EWDPresidentialViewCategory::None;
	}

	if (Actor->Implements<UWDInteractable>())
	{
		const FGameplayTagContainer Tags = IWDInteractable::Execute_GetInteractionTags(Actor);
		if (Tags.HasTag(WDGameplayTags::PresidentialView_Diplomatic)) return EWDPresidentialViewCategory::Diplomatic;
		if (Tags.HasTag(WDGameplayTags::PresidentialView_Security))  return EWDPresidentialViewCategory::Security;
		if (Tags.HasTag(WDGameplayTags::PresidentialView_Vehicle))   return EWDPresidentialViewCategory::Vehicle;
		if (Tags.HasTag(WDGameplayTags::PresidentialView_Intel))     return EWDPresidentialViewCategory::Intel;
		if (Tags.HasTag(WDGameplayTags::PresidentialView_Person))    return EWDPresidentialViewCategory::Person;
	}

	if (Actor->Implements<UWDDiplomaticActor>())
	{
		return EWDPresidentialViewCategory::Diplomatic;
	}

	if (Cast<AWDSecurityAgent>(Actor))
	{
		return EWDPresidentialViewCategory::Security;
	}

	if (Actor->Implements<UWDMissionTarget>())
	{
		return EWDPresidentialViewCategory::Objective;
	}

	return EWDPresidentialViewCategory::None;
}

void UWDPresidentialViewComponent::RescanNearbyActors()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Clear the previous scan's highlights before applying the new one.
	for (const TWeakObjectPtr<AActor>& WeakActor : HighlightedActors)
	{
		if (AActor* Actor = WeakActor.Get())
		{
			TArray<UPrimitiveComponent*> Prims;
			Actor->GetComponents<UPrimitiveComponent>(Prims);
			for (UPrimitiveComponent* Prim : Prims)
			{
				Prim->SetRenderCustomDepth(false);
			}
		}
	}
	HighlightedActors.Reset();

	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(ScanRadius);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	if (Owner->GetWorld()->OverlapMultiByChannel(Overlaps, Owner->GetActorLocation(), FQuat::Identity, ECC_Pawn, Sphere, Params))
	{
		for (const FOverlapResult& Overlap : Overlaps)
		{
			AActor* Candidate = Overlap.GetActor();
			const EWDPresidentialViewCategory Category = ClassifyActor(Candidate);
			if (Category == EWDPresidentialViewCategory::None)
			{
				continue;
			}

			const int32 Stencil = UWDPresidentialViewStencils::GetStencilValue(Category);
			TArray<UPrimitiveComponent*> Prims;
			Candidate->GetComponents<UPrimitiveComponent>(Prims);
			for (UPrimitiveComponent* Prim : Prims)
			{
				Prim->SetRenderCustomDepth(true);
				Prim->SetCustomDepthStencilValue(Stencil);
			}
			HighlightedActors.Add(Candidate);
		}
	}
}
