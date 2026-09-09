#include "WDInteractionComponent.h"
#include "IWDInteractable.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

UWDInteractionComponent::UWDInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UWDInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!OwningPawn)
	{
		return;
	}

	const UCameraComponent* Camera = OwningPawn->FindComponentByClass<UCameraComponent>();
	const FVector TraceStart = Camera ? Camera->GetComponentLocation() : OwningPawn->GetActorLocation();
	const FVector TraceDir = Camera ? Camera->GetForwardVector() : OwningPawn->GetActorForwardVector();
	const FVector TraceEnd = TraceStart + TraceDir * TraceDistance;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwningPawn);

	TArray<FHitResult> Hits;
	GetWorld()->SweepMultiByChannel(Hits, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility,
		FCollisionShape::MakeSphere(TraceRadius), Params);

	AActor* BestCandidate = nullptr;
	for (const FHitResult& Hit : Hits)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UWDInteractable>())
		{
			if (IWDInteractable::Execute_CanInteract(HitActor, OwningPawn))
			{
				BestCandidate = HitActor;
				break;
			}
		}
	}

	if (BestCandidate != FocusedInteractable.Get())
	{
		FocusedInteractable = BestCandidate;
		OnFocusedInteractableChanged.Broadcast(BestCandidate);
	}
}

void UWDInteractionComponent::TryInteract()
{
	if (AActor* Target = FocusedInteractable.Get())
	{
		if (Target->Implements<UWDInteractable>())
		{
			IWDInteractable::Execute_OnInteract(Target, GetOwner());
		}
	}
}
