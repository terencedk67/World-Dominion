#include "WDGovernmentNPC.h"
#include "WorldDominion.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WDGameplayTags.h"
#include "WDMissionSubsystem.h"
#include "Kismet/GameplayStatics.h"

AWDGovernmentNPC::AWDGovernmentNPC()
{
	GetCapsuleComponent()->InitCapsuleSize(38.f, 92.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	PlaceholderBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBodyMesh"));
	PlaceholderBodyMesh->SetupAttachment(RootComponent);
	if (PlaceholderMeshFinder.Succeeded())
	{
		PlaceholderBodyMesh->SetStaticMesh(PlaceholderMeshFinder.Object);
	}
	PlaceholderBodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	PlaceholderBodyMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.7f));
	GetMesh()->SetVisibility(false);
}

FText AWDGovernmentNPC::GetInteractionPrompt_Implementation() const
{
	return FText::Format(NSLOCTEXT("WorldDominion", "TalkToPrompt", "Talk to {0}"), Profile.RoleTitle);
}

FGameplayTagContainer AWDGovernmentNPC::GetInteractionTags_Implementation() const
{
	FGameplayTagContainer Tags;
	Tags.AddTag(WDGameplayTags::Interact_Talk);
	Tags.AddTag(WDGameplayTags::PresidentialView_Person);
	return Tags;
}

bool AWDGovernmentNPC::CanInteract_Implementation(AActor* Instigator) const
{
	return true;
}

void AWDGovernmentNPC::OnInteract_Implementation(AActor* Instigator)
{
	if (DialogueLines.IsValidIndex(NextDialogueIndex))
	{
		OnDialogueLine(Profile.DisplayName, DialogueLines[NextDialogueIndex]);
		++NextDialogueIndex;
	}
	else if (DialogueLines.Num() > 0)
	{
		OnDialogueLine(Profile.DisplayName, DialogueLines.Last());
	}

	if (!SatisfiesObjectiveID.IsNone())
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
		{
			if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
			{
				Missions->CompleteObjective(SatisfiesObjectiveID);
			}
		}
	}
}

void AWDGovernmentNPC::NotifyObjectiveSatisfied_Implementation(AActor* Instigator)
{
	// This NPC *causes* objective completion via OnInteract rather than reacting to one -
	// left as a no-op hook for objectives where a different system (e.g. a scripted event)
	// needs to notify this NPC instead (triggering a new line of dialogue, for example).
}
