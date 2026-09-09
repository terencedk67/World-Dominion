#include "WDIntelligenceSubsystem.h"

void UWDIntelligenceSubsystem::PublishBriefing(const FWDIntelBriefing& Briefing)
{
	Briefings.Add(Briefing);
}
