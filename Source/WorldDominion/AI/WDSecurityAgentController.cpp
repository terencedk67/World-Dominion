#include "WDSecurityAgentController.h"
#include "WDSecurityAgent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"

AWDSecurityAgentController::AWDSecurityAgentController()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2500.f;
	SightConfig->LoseSightRadius = 3000.f;
	SightConfig->PeripheralVisionAngleDegrees = 100.f;
	SightConfig->MaxAge = 5.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AWDSecurityAgentController::BeginPlay()
{
	Super::BeginPlay();

	if (UAIPerceptionComponent* Perception = GetPerceptionComponent())
	{
		Perception->OnTargetPerceptionUpdated.AddDynamic(this, &AWDSecurityAgentController::HandlePerceptionUpdated);
	}
}

void AWDSecurityAgentController::HandlePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (AWDSecurityAgent* Agent = Cast<AWDSecurityAgent>(GetPawn()))
	{
		// Phase 1: no hostility classification system yet - every fresh sighting is
		// forwarded as non-hostile. A later Intelligence/threat-assessment pass can
		// set bIsHostile from a crowd/NPC alignment tag.
		Agent->OnPerceivedActor(Actor, false);
	}
}
