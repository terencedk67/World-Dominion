// Phase 1/2 scaffold for the Intelligence pillar (operations, briefings,
// IntelligenceConfidence between nations). IntelligenceConfidence already
// exists as a relationship dimension in FWDRelationshipState and
// IntelligencePower as a country stat, so briefings/missions can consume
// those today; this subsystem is where covert operations and generated
// intel briefings (feeding the ReviewDocument objective type) land in
// Phase 4.
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WDIntelligenceSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FWDIntelBriefing
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intelligence")
	FText Headline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intelligence")
	FText Body;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Intelligence")
	FName RelatedCountryID;
};

UCLASS()
class WORLDDOMINION_API UWDIntelligenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Intelligence")
	void PublishBriefing(const FWDIntelBriefing& Briefing);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|Intelligence")
	const TArray<FWDIntelBriefing>& GetBriefings() const { return Briefings; }

protected:
	UPROPERTY(Transient)
	TArray<FWDIntelBriefing> Briefings;
};
