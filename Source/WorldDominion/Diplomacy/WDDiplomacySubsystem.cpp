#include "WDDiplomacySubsystem.h"
#include "WorldDominion.h"
#include "WDEconomySubsystem.h"
#include "WDCountrySubsystem.h"
#include "WDCountryStats.h"

namespace
{
	/** One action's default consequence shape: how it moves the acting/target relationship
	 *  dimensions, and the economic knock-on applied to each side. Defined once per action
	 *  type (never per country) - e.g. ImposeSanctions always damages Trust and
	 *  EconomicDependency and dents both nations' Treasury, whoever A and B happen to be. */
	struct FActionOutcomeTemplate
	{
		float TrustDelta = 0.f;
		float EconomicDependencyDelta = 0.f;
		float MilitaryCooperationDelta = 0.f;
		float PublicSentimentDelta = 0.f;
		float HistoricalTensionDelta = 0.f;
		float LeadershipRelationshipDelta = 0.f;
		float IntelligenceConfidenceDelta = 0.f;

		float ActingTreasuryDelta = 0.f;
		float TargetTreasuryDelta = 0.f;
		float ActingApprovalDelta = 0.f;

		FString HeadlineFormat; // {0}=Acting country, {1}=Target country
	};

	FActionOutcomeTemplate GetOutcomeTemplate(EWDDiplomaticAction Action)
	{
		FActionOutcomeTemplate Outcome;
		switch (Action)
		{
		case EWDDiplomaticAction::CallLeader:
			Outcome.TrustDelta = 3.f;
			Outcome.LeadershipRelationshipDelta = 5.f;
			Outcome.HeadlineFormat = TEXT("{0} AND {1} LEADERS HOLD PHONE CALL");
			break;
		case EWDDiplomaticAction::RequestSummit:
			Outcome.TrustDelta = 2.f;
			Outcome.LeadershipRelationshipDelta = 2.f;
			Outcome.HeadlineFormat = TEXT("{0} REQUESTS SUMMIT WITH {1}");
			break;
		case EWDDiplomaticAction::VisitCountry:
			Outcome.TrustDelta = 6.f;
			Outcome.PublicSentimentDelta = 4.f;
			Outcome.ActingApprovalDelta = 1.f;
			Outcome.HeadlineFormat = TEXT("{0} LEADER ARRIVES IN {1} FOR STATE VISIT");
			break;
		case EWDDiplomaticAction::ProposeTradeDeal:
			Outcome.EconomicDependencyDelta = 8.f;
			Outcome.TrustDelta = 3.f;
			Outcome.ActingTreasuryDelta = 5000000.0;
			Outcome.TargetTreasuryDelta = 5000000.0;
			Outcome.HeadlineFormat = TEXT("{0} AND {1} ANNOUNCE NEW TRADE AGREEMENT");
			break;
		case EWDDiplomaticAction::FormAlliance:
			Outcome.TrustDelta = 15.f;
			Outcome.MilitaryCooperationDelta = 15.f;
			Outcome.HistoricalTensionDelta = -5.f;
			Outcome.HeadlineFormat = TEXT("{0} AND {1} FORMALIZE ALLIANCE");
			break;
		case EWDDiplomaticAction::ImposeSanctions:
			Outcome.TrustDelta = -20.f;
			Outcome.EconomicDependencyDelta = -15.f;
			Outcome.HistoricalTensionDelta = 10.f;
			Outcome.ActingTreasuryDelta = -2000000.0;
			Outcome.TargetTreasuryDelta = -8000000.0;
			Outcome.ActingApprovalDelta = 2.f; // domestic industries approve
			Outcome.HeadlineFormat = TEXT("{0} IMPOSES SANCTIONS ON {1}");
			break;
		case EWDDiplomaticAction::RemoveSanctions:
			Outcome.TrustDelta = 10.f;
			Outcome.EconomicDependencyDelta = 8.f;
			Outcome.HeadlineFormat = TEXT("{0} LIFTS SANCTIONS ON {1}");
			break;
		case EWDDiplomaticAction::ShareIntelligence:
			Outcome.IntelligenceConfidenceDelta = 12.f;
			Outcome.TrustDelta = 5.f;
			Outcome.HeadlineFormat = TEXT("{0} AND {1} DEEPEN INTELLIGENCE COOPERATION");
			break;
		case EWDDiplomaticAction::RequestMilitaryAccess:
			Outcome.MilitaryCooperationDelta = 10.f;
			Outcome.HistoricalTensionDelta = 3.f;
			Outcome.HeadlineFormat = TEXT("{0} REQUESTS MILITARY ACCESS FROM {1}");
			break;
		case EWDDiplomaticAction::IssuePublicStatement:
			Outcome.PublicSentimentDelta = 2.f;
			Outcome.HeadlineFormat = TEXT("{0} ISSUES PUBLIC STATEMENT ON RELATIONS WITH {1}");
			break;
		}
		return Outcome;
	}
}

FString UWDDiplomacySubsystem::MakePairKey(FName CountryA, FName CountryB)
{
	// Order-independent key so A->B and B->A share one relationship record.
	return CountryA.LexicalLess(CountryB)
		? FString::Printf(TEXT("%s|%s"), *CountryA.ToString(), *CountryB.ToString())
		: FString::Printf(TEXT("%s|%s"), *CountryB.ToString(), *CountryA.ToString());
}

FWDRelationshipState UWDDiplomacySubsystem::GetRelationship(FName CountryA, FName CountryB) const
{
	if (const FWDRelationshipState* Found = Relationships.Find(MakePairKey(CountryA, CountryB)))
	{
		return *Found;
	}
	return FWDRelationshipState();
}

float UWDDiplomacySubsystem::ModifyRelationship(FName CountryA, FName CountryB, FName Dimension, float Delta)
{
	FWDRelationshipState& State = Relationships.FindOrAdd(MakePairKey(CountryA, CountryB));
	float NewValue = 0.f;
	State.ModifyDimension(Dimension, Delta, NewValue);
	OnRelationshipChanged.Broadcast(CountryA, CountryB, State);
	return NewValue;
}

FText UWDDiplomacySubsystem::ApplyDiplomaticAction(FName CountryA, FName CountryB, EWDDiplomaticAction Action)
{
	const FActionOutcomeTemplate Outcome = GetOutcomeTemplate(Action);

	ModifyRelationship(CountryA, CountryB, TEXT("Trust"), Outcome.TrustDelta);
	ModifyRelationship(CountryA, CountryB, TEXT("EconomicDependency"), Outcome.EconomicDependencyDelta);
	ModifyRelationship(CountryA, CountryB, TEXT("MilitaryCooperation"), Outcome.MilitaryCooperationDelta);
	ModifyRelationship(CountryA, CountryB, TEXT("PublicSentiment"), Outcome.PublicSentimentDelta);
	ModifyRelationship(CountryA, CountryB, TEXT("HistoricalTension"), Outcome.HistoricalTensionDelta);
	ModifyRelationship(CountryA, CountryB, TEXT("LeadershipRelationship"), Outcome.LeadershipRelationshipDelta);
	ModifyRelationship(CountryA, CountryB, TEXT("IntelligenceConfidence"), Outcome.IntelligenceConfidenceDelta);

	if (UWDEconomySubsystem* Economy = GetGameInstance()->GetSubsystem<UWDEconomySubsystem>())
	{
		TArray<FWDStatEffect> Effects;
		if (Outcome.ActingTreasuryDelta != 0.f)
		{
			Effects.Add(FWDStatEffect{ TEXT("Treasury"), CountryA, Outcome.ActingTreasuryDelta });
		}
		if (Outcome.TargetTreasuryDelta != 0.f)
		{
			Effects.Add(FWDStatEffect{ TEXT("Treasury"), CountryB, Outcome.TargetTreasuryDelta });
		}
		if (Outcome.ActingApprovalDelta != 0.f)
		{
			Effects.Add(FWDStatEffect{ TEXT("PublicApproval"), CountryA, Outcome.ActingApprovalDelta });
		}
		Economy->ApplyStatEffects(Effects, CountryA);
	}

	const FString AName = CountryA.ToString().ToUpper();
	const FString BName = CountryB.ToString().ToUpper();
	const FText Headline = FText::FromString(FString::Format(*Outcome.HeadlineFormat, { AName, BName }));

	OnDiplomaticHeadline.Broadcast(Headline);
	UE_LOG(LogWorldDominion, Log, TEXT("Diplomacy: %s"), *Headline.ToString());

	return Headline;
}
