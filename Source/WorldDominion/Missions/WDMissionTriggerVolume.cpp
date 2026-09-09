#include "WDMissionTriggerVolume.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "WDMissionSubsystem.h"
#include "WDWorldEventSubsystem.h"
#include "Kismet/GameplayStatics.h"

AWDMissionTriggerVolume::AWDMissionTriggerVolume()
{
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	SetRootComponent(TriggerBox);
	TriggerBox->SetBoxExtent(FVector(150.f, 150.f, 100.f));
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
}

void AWDMissionTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AWDMissionTriggerVolume::HandleBeginOverlap);
}

void AWDMissionTriggerVolume::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTriggerOnce && bHasFired)
	{
		return;
	}

	if (!OtherActor || !OtherActor->IsA<APawn>() || !Cast<APawn>(OtherActor)->IsPlayerControlled())
	{
		return;
	}

	bHasFired = true;

	if (ObjectiveIDs.Num() > 0)
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
		{
			if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
			{
				for (FName ObjectiveID : ObjectiveIDs)
				{
					if (!ObjectiveID.IsNone())
					{
						Missions->CompleteObjective(ObjectiveID);
					}
				}
			}
		}
	}

	if (!EventIDToTrigger.IsNone())
	{
		if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
		{
			if (UWDWorldEventSubsystem* Events = GI->GetSubsystem<UWDWorldEventSubsystem>())
			{
				Events->TriggerEvent(EventIDToTrigger);
			}
		}
	}
}
