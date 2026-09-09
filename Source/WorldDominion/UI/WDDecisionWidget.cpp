#include "WDDecisionWidget.h"
#include "WDWorldEventSubsystem.h"
#include "WDGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UWDDecisionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDWorldEventSubsystem* Events = GI->GetSubsystem<UWDWorldEventSubsystem>())
		{
			Events->OnWorldEventTriggered.AddDynamic(this, &UWDDecisionWidget::HandleWorldEventTriggered);
		}
	}
}

void UWDDecisionWidget::NativeDestruct()
{
	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDWorldEventSubsystem* Events = GI->GetSubsystem<UWDWorldEventSubsystem>())
		{
			Events->OnWorldEventTriggered.RemoveDynamic(this, &UWDDecisionWidget::HandleWorldEventTriggered);
		}
	}
	Super::NativeDestruct();
}

void UWDDecisionWidget::HandleWorldEventTriggered(FWDWorldEventDefinition Event)
{
	CurrentEventID = Event.EventID;
	OnDecisionPresented(Event);
	SetVisibility(ESlateVisibility::Visible);
}

void UWDDecisionWidget::ChooseOption(int32 OptionIndex)
{
	if (CurrentEventID.IsNone())
	{
		return;
	}

	if (UGameInstance* GI = UGameplayStatics::GetGameInstance(this))
	{
		if (UWDWorldEventSubsystem* Events = GI->GetSubsystem<UWDWorldEventSubsystem>())
		{
			const UWDGameInstance* WDGI = Cast<UWDGameInstance>(GI);
			const FName ActingCountry = WDGI ? WDGI->GetSelectedCountry() : NAME_None;

			FWDWorldEventDefinition Definition;
			Events->GetPendingEvent(CurrentEventID, Definition);

			if (Events->ResolveEvent(CurrentEventID, OptionIndex, ActingCountry) && Definition.Options.IsValidIndex(OptionIndex))
			{
				OnDecisionResolved(Definition.Options[OptionIndex].ResultingHeadline);
			}
		}
	}

	CurrentEventID = NAME_None;
	SetVisibility(ESlateVisibility::Collapsed);
}
