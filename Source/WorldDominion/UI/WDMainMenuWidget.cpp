#include "WDMainMenuWidget.h"
#include "WDSaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UWDMainMenuWidget::StartNewGame()
{
	if (!NationSelectLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, NationSelectLevel);
	}
}

bool UWDMainMenuWidget::HasSaveGame() const
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDSaveSubsystem* Save = GI->GetSubsystem<UWDSaveSubsystem>())
		{
			return Save->DoesSaveExist();
		}
	}
	return false;
}

void UWDMainMenuWidget::ContinueGame()
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDSaveSubsystem* Save = GI->GetSubsystem<UWDSaveSubsystem>())
		{
			Save->LoadWorldState();
		}
	}
}

void UWDMainMenuWidget::QuitGame()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
