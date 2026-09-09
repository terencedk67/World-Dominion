#include "WDGameMode.h"
#include "WorldDominion.h"
#include "WDPlayerController.h"
#include "WDGameState.h"
#include "WDPlayerCharacter.h"
#include "WDHUD.h"
#include "WDMissionSubsystem.h"

AWDGameMode::AWDGameMode()
{
	DefaultPawnClass = AWDPlayerCharacter::StaticClass();
	PlayerControllerClass = AWDPlayerController::StaticClass();
	GameStateClass = AWDGameState::StaticClass();
	HUDClass = AWDHUD::StaticClass();
}

void AWDGameMode::StartPlay()
{
	Super::StartPlay();

	if (AWDGameState* WDGameState = GetGameState<AWDGameState>())
	{
		WDGameState->RefreshFromCountrySubsystem();
	}

	if (!OpeningMissionID.IsNone())
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
			{
				Missions->StartMission(OpeningMissionID);
			}
		}
	}
}
