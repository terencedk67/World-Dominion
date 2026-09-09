#include "WDCountryStats.h"

// Fixed, well-known set of numeric simulation channels that FWDStatEffect (decisions,
// world events, diplomatic actions) is allowed to target by name. Adding a new *country*
// never touches this list; adding a new *kind of number* to the simulation does, exactly
// once, here.
bool FWDCountryStats::ModifyNumericStat(FName StatID, float Delta, float& OutNewValue)
{
	if (StatID == TEXT("Treasury"))				{ Treasury += Delta; OutNewValue = static_cast<float>(Treasury); return true; }
	if (StatID == TEXT("GDP"))						{ GDP += Delta; OutNewValue = static_cast<float>(GDP); return true; }
	if (StatID == TEXT("DiplomaticInfluence"))		{ DiplomaticInfluence += Delta; OutNewValue = DiplomaticInfluence; return true; }
	if (StatID == TEXT("PublicApproval"))			{ PublicApproval = FMath::Clamp(PublicApproval + Delta, 0.f, 100.f); OutNewValue = PublicApproval; return true; }
	if (StatID == TEXT("Stability"))				{ Stability = FMath::Clamp(Stability + Delta, 0.f, 100.f); OutNewValue = Stability; return true; }
	if (StatID == TEXT("MilitaryPower"))			{ MilitaryPower = FMath::Clamp(MilitaryPower + Delta, 0.f, 100.f); OutNewValue = MilitaryPower; return true; }
	if (StatID == TEXT("IntelligencePower"))		{ IntelligencePower = FMath::Clamp(IntelligencePower + Delta, 0.f, 100.f); OutNewValue = IntelligencePower; return true; }
	if (StatID == TEXT("Population"))				{ Population += static_cast<int64>(Delta); OutNewValue = static_cast<float>(Population); return true; }

	OutNewValue = 0.f;
	return false;
}

bool FWDCountryStats::GetNumericStat(FName StatID, float& OutValue) const
{
	if (StatID == TEXT("Treasury"))				{ OutValue = static_cast<float>(Treasury); return true; }
	if (StatID == TEXT("GDP"))						{ OutValue = static_cast<float>(GDP); return true; }
	if (StatID == TEXT("DiplomaticInfluence"))		{ OutValue = DiplomaticInfluence; return true; }
	if (StatID == TEXT("PublicApproval"))			{ OutValue = PublicApproval; return true; }
	if (StatID == TEXT("Stability"))				{ OutValue = Stability; return true; }
	if (StatID == TEXT("MilitaryPower"))			{ OutValue = MilitaryPower; return true; }
	if (StatID == TEXT("IntelligencePower"))		{ OutValue = IntelligencePower; return true; }
	if (StatID == TEXT("Population"))				{ OutValue = static_cast<float>(Population); return true; }

	OutValue = 0.f;
	return false;
}
