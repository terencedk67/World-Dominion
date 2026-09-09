#include "WDCountryTravelNode.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "WDGameplayTags.h"
#include "WDTravelSubsystem.h"
#include "Kismet/GameplayStatics.h"

AWDCountryTravelNode::AWDCountryTravelNode()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	PlaceholderMarkerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMarkerMesh"));
	SetRootComponent(PlaceholderMarkerMesh);
	if (PlaceholderMeshFinder.Succeeded())
	{
		PlaceholderMarkerMesh->SetStaticMesh(PlaceholderMeshFinder.Object);
	}
	PlaceholderMarkerMesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.05f));
}

FText AWDCountryTravelNode::GetInteractionPrompt_Implementation() const
{
	return FText::Format(NSLOCTEXT("WorldDominion", "TravelPrompt", "Travel to {0}"), DestinationDisplayName);
}

FGameplayTagContainer AWDCountryTravelNode::GetInteractionTags_Implementation() const
{
	FGameplayTagContainer Tags;
	Tags.AddTag(WDGameplayTags::Interact_Use);
	Tags.AddTag(WDGameplayTags::PresidentialView_Vehicle);
	return Tags;
}

void AWDCountryTravelNode::OnInteract_Implementation(AActor* Instigator)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDTravelSubsystem* Travel = GI->GetSubsystem<UWDTravelSubsystem>())
		{
			Travel->TravelTo(DestinationCountryID);
		}
	}
}
