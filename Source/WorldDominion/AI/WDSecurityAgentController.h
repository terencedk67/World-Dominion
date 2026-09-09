// AIController for AWDSecurityAgent. Owns the perception stack ("scan crowds")
// and drives pathing via the standard navmesh MoveTo. Kept intentionally thin:
// the agent's own state machine (AWDSecurityAgent) decides *what* to do,
// this class only decides *how* to move and *what* it can currently see.
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WDSecurityAgentController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class WORLDDOMINION_API AWDSecurityAgentController : public AAIController
{
	GENERATED_BODY()

public:
	AWDSecurityAgentController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "WorldDominion|Security")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	UFUNCTION()
	void HandlePerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);
};
