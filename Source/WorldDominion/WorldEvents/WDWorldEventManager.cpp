#include "WDWorldEventManager.h"
#include "WorldDominion.h"
#include "WDWorldEventSubsystem.h"
#include "Kismet/GameplayStatics.h"

AWDWorldEventManager::AWDWorldEventManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool AWDWorldEventManager::TriggerNamedEvent(FName EventID)
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDWorldEventSubsystem* Events = GI->GetSubsystem<UWDWorldEventSubsystem>())
		{
			return Events->TriggerEvent(EventID);
		}
	}
	return false;
}

void AWDWorldEventManager::StartRandomEventPacing(float MinIntervalSeconds, float MaxIntervalSeconds)
{
	UE_LOG(LogWorldDominion, Verbose, TEXT("AWDWorldEventManager::StartRandomEventPacing is a Phase 4 stub (requested range %.0f-%.0fs)."), MinIntervalSeconds, MaxIntervalSeconds);
}
