// Shared enums and lightweight value types used across every WORLD DOMINION
// subsystem. Kept engine-agnostic of any single country/mission/event so new
// content is added as data (CSV/JSON/DataAssets), never as new switch cases.
#pragma once

#include "CoreMinimal.h"
#include "WDTypes.generated.h"

/** Government archetypes. Drives approval-model branching (elections vs. elite support). */
UENUM(BlueprintType)
enum class EWDGovernmentType : uint8
{
	Democracy		UMETA(DisplayName = "Democracy"),
	Monarchy		UMETA(DisplayName = "Monarchy"),
	Authoritarian	UMETA(DisplayName = "Authoritarian"),
	OneParty		UMETA(DisplayName = "One-Party State"),
	Federation		UMETA(DisplayName = "Federation")
};

/** Overall diplomatic posture bucket between two nations, derived from FWDRelationshipState. */
UENUM(BlueprintType)
enum class EWDRelationStanding : uint8
{
	Ally			UMETA(DisplayName = "Ally"),
	Friendly		UMETA(DisplayName = "Friendly"),
	Neutral			UMETA(DisplayName = "Neutral"),
	Tense			UMETA(DisplayName = "Tense"),
	Hostile			UMETA(DisplayName = "Hostile")
};

/** Presidential protection posture. Drives UWDPresidentialSecurityComponent formation/behavior. */
UENUM(BlueprintType)
enum class EWDProtectionState : uint8
{
	Normal			UMETA(DisplayName = "Normal"),
	Heightened		UMETA(DisplayName = "Heightened"),
	Emergency		UMETA(DisplayName = "Emergency"),
	Lockdown		UMETA(DisplayName = "Lockdown")
};

/** Reusable mission objective archetypes. New missions compose these instead of new code. */
UENUM(BlueprintType)
enum class EWDObjectiveType : uint8
{
	GoTo			UMETA(DisplayName = "Go To"),
	TalkTo			UMETA(DisplayName = "Talk To"),
	Interact		UMETA(DisplayName = "Interact"),
	EnterVehicle	UMETA(DisplayName = "Enter Vehicle"),
	Travel			UMETA(DisplayName = "Travel"),
	AttendMeeting	UMETA(DisplayName = "Attend Meeting"),
	MakeDecision	UMETA(DisplayName = "Make Decision"),
	Wait			UMETA(DisplayName = "Wait"),
	Escort			UMETA(DisplayName = "Escort"),
	ReviewDocument	UMETA(DisplayName = "Review Document"),
	GiveSpeech		UMETA(DisplayName = "Give Speech"),
	VisitLocation	UMETA(DisplayName = "Visit Location")
};

UENUM(BlueprintType)
enum class EWDObjectiveState : uint8
{
	Inactive,
	Active,
	Completed,
	Failed
};

/** World event domains, matches the Event.* gameplay tag root. */
UENUM(BlueprintType)
enum class EWDEventCategory : uint8
{
	Economic,
	Military,
	Diplomatic,
	Environmental,
	Political,
	Social,
	Technological,
	Intelligence,
	Energy,
	Trade
};

/** Diplomatic / executive actions the player can issue from the World Map or Phone. */
UENUM(BlueprintType)
enum class EWDDiplomaticAction : uint8
{
	CallLeader,
	RequestSummit,
	VisitCountry,
	ProposeTradeDeal,
	FormAlliance,
	ImposeSanctions,
	RemoveSanctions,
	ShareIntelligence,
	RequestMilitaryAccess,
	IssuePublicStatement
};

/** A single named, numeric outcome applied by a decision option or event resolution.
 *  Kept generic (StatID + Delta) so new decisions never require new C++ effect types -
 *  UWDEconomySubsystem / UWDDiplomacySubsystem / UWDCountrySubsystem all know how to
 *  route a small, well-known set of StatIDs (see WDGameplayTags Stat.* namespace). */
USTRUCT(BlueprintType)
struct FWDStatEffect
{
	GENERATED_BODY()

	/** e.g. "Treasury", "Approval", "Relation.Trust", "Stability" - resolved by the owning subsystem. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName StatID;

	/** Country this effect applies to. NAME_None means "the player's own country". */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName TargetCountryID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	float Delta = 0.f;
};

/** One selectable branch of a Decision (world event resolution or mission choice). */
USTRUCT(BlueprintType)
struct FWDDecisionOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision")
	FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision")
	TArray<FWDStatEffect> Effects;

	/** Optional headline template shown by the media system when this option is chosen. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Decision")
	FText ResultingHeadline;
};
