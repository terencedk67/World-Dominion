#include "WDPlayerController.h"
#include "WorldDominion.h"
#include "WDPresidentialSecurityComponent.h"
#include "WDHUDWidget.h"
#include "Blueprint/UserWidget.h"

AWDPlayerController::AWDPlayerController()
{
	PresidentialSecurity = CreateDefaultSubobject<UWDPresidentialSecurityComponent>(TEXT("PresidentialSecurity"));
}

void AWDPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UWDHUDWidget>(this, HUDWidgetClass);
	}
	else
	{
		// Base UUserWidget-compatible fallback so the HUD backing logic runs even before
		// a WBP_HUD Blueprint child (with real UMG layout) is authored in-editor.
		HUDWidget = CreateWidget<UWDHUDWidget>(this, UWDHUDWidget::StaticClass());
	}

	if (HUDWidget)
	{
		HUDWidget->AddToViewport(0);
	}
	else
	{
		UE_LOG(LogWorldDominion, Warning, TEXT("AWDPlayerController: failed to create HUD widget."));
	}
}

void AWDPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (PresidentialSecurity)
	{
		PresidentialSecurity->OnProtecteePossessed(InPawn);
	}
}
