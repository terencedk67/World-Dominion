// The eight relationship dimensions two nations track against each other.
// Every diplomatic action (tariffs, sanctions, summits, intelligence sharing...)
// nudges one or more of these; UWDDiplomacySubsystem derives the coarse
// EWDRelationStanding bucket from the aggregate for UI display.
#pragma once

#include "CoreMinimal.h"
#include "WDTypes.h"
#include "WDRelationshipTypes.generated.h"

USTRUCT(BlueprintType)
struct FWDRelationshipState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float Trust = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float EconomicDependency = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float MilitaryCooperation = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float PublicSentiment = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float HistoricalTension = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float IdeologicalCompatibility = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float LeadershipRelationship = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Relationship", meta = (ClampMin = "-100", ClampMax = "100"))
	float IntelligenceConfidence = 0.f;

	/** Straight average of all eight dimensions, bucketed into the display-facing standing. */
	EWDRelationStanding GetOverallStanding() const
	{
		const float Average = (Trust + EconomicDependency + MilitaryCooperation + PublicSentiment
			- HistoricalTension + IdeologicalCompatibility + LeadershipRelationship + IntelligenceConfidence) / 8.f;

		if (Average >= 60.f)	return EWDRelationStanding::Ally;
		if (Average >= 20.f)	return EWDRelationStanding::Friendly;
		if (Average >= -20.f)	return EWDRelationStanding::Neutral;
		if (Average >= -60.f)	return EWDRelationStanding::Tense;
		return EWDRelationStanding::Hostile;
	}

	bool ModifyDimension(FName Dimension, float Delta, float& OutNewValue)
	{
		float* Field = nullptr;
		if (Dimension == TEXT("Trust")) Field = &Trust;
		else if (Dimension == TEXT("EconomicDependency")) Field = &EconomicDependency;
		else if (Dimension == TEXT("MilitaryCooperation")) Field = &MilitaryCooperation;
		else if (Dimension == TEXT("PublicSentiment")) Field = &PublicSentiment;
		else if (Dimension == TEXT("HistoricalTension")) Field = &HistoricalTension;
		else if (Dimension == TEXT("IdeologicalCompatibility")) Field = &IdeologicalCompatibility;
		else if (Dimension == TEXT("LeadershipRelationship")) Field = &LeadershipRelationship;
		else if (Dimension == TEXT("IntelligenceConfidence")) Field = &IntelligenceConfidence;

		if (!Field)
		{
			OutNewValue = 0.f;
			return false;
		}

		*Field = FMath::Clamp(*Field + Delta, -100.f, 100.f);
		OutNewValue = *Field;
		return true;
	}
};
