#include "WDHUDWidget.h"
#include "WDGameState.h"
#include "WDMissionSubsystem.h"
#include "WDDiplomacySubsystem.h"
#include "WDPlayerCharacter.h"
#include "WDInteractionComponent.h"
#include "WDPresidentialViewComponent.h"
#include "IWDInteractable.h"
#include "Kismet/GameplayStatics.h"

void UWDHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (AWDGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AWDGameState>() : nullptr)
	{
		GameState->OnStatsChangedDelegate.AddDynamic(this, &UWDHUDWidget::HandleTopLineStatsChanged);
		HandleTopLineStatsChanged();
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
		{
			Missions->OnObjectiveStateChanged.AddDynamic(this, &UWDHUDWidget::HandleObjectiveStateChanged);
		}

		if (UWDDiplomacySubsystem* Diplomacy = GI->GetSubsystem<UWDDiplomacySubsystem>())
		{
			Diplomacy->OnDiplomaticHeadline.AddDynamic(this, &UWDHUDWidget::HandleDiplomaticHeadline);
		}
	}

	if (AWDPlayerCharacter* Character = Cast<AWDPlayerCharacter>(GetOwningPlayerPawn()))
	{
		if (Character->InteractionComponent)
		{
			Character->InteractionComponent->OnFocusedInteractableChanged.AddDynamic(this, &UWDHUDWidget::HandleFocusedInteractableChanged);
		}
		if (Character->PresidentialViewComponent)
		{
			Character->PresidentialViewComponent->OnToggled.AddDynamic(this, &UWDHUDWidget::HandlePresidentialViewToggled);
		}
	}

	RefreshObjectives();
}

void UWDHUDWidget::NativeDestruct()
{
	if (AWDGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AWDGameState>() : nullptr)
	{
		GameState->OnStatsChangedDelegate.RemoveDynamic(this, &UWDHUDWidget::HandleTopLineStatsChanged);
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
		{
			Missions->OnObjectiveStateChanged.RemoveDynamic(this, &UWDHUDWidget::HandleObjectiveStateChanged);
		}
		if (UWDDiplomacySubsystem* Diplomacy = GI->GetSubsystem<UWDDiplomacySubsystem>())
		{
			Diplomacy->OnDiplomaticHeadline.RemoveDynamic(this, &UWDHUDWidget::HandleDiplomaticHeadline);
		}
	}

	Super::NativeDestruct();
}

void UWDHUDWidget::HandleTopLineStatsChanged()
{
	if (AWDGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AWDGameState>() : nullptr)
	{
		OnTopLineStatsUpdated(GameState->Influence, GameState->Treasury, GameState->AllyCount, GameState->PublicApproval);
	}
}

void UWDHUDWidget::HandleObjectiveStateChanged(FName ObjectiveID, EWDObjectiveState NewState)
{
	RefreshObjectives();
}

void UWDHUDWidget::RefreshObjectives()
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDMissionSubsystem* Missions = GI->GetSubsystem<UWDMissionSubsystem>())
		{
			OnObjectivesUpdated(Missions->GetAllActiveObjectives());
		}
	}
}

void UWDHUDWidget::HandleFocusedInteractableChanged(AActor* NewFocus)
{
	if (NewFocus && NewFocus->Implements<UWDInteractable>())
	{
		OnInteractionPromptUpdated(IWDInteractable::Execute_GetInteractionPrompt(NewFocus), true);
	}
	else
	{
		OnInteractionPromptUpdated(FText::GetEmpty(), false);
	}
}

void UWDHUDWidget::HandlePresidentialViewToggled(bool bIsActive)
{
	OnPresidentialViewToggled(bIsActive);
}

void UWDHUDWidget::HandleDiplomaticHeadline(FText Headline)
{
	OnHeadline(Headline);
}
