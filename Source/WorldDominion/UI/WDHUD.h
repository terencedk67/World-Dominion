// Base AHUD. The actual HUD widget (UWDHUDWidget) is created and owned by
// AWDPlayerController so it survives pawn changes; this class exists mainly
// so AWDGameMode has a concrete HUD class to assign and as a hook point for
// any future debug/DrawHUD overlays (e.g. a designer-only stat readout).
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WDHUD.generated.h"

UCLASS()
class WORLDDOMINION_API AWDHUD : public AHUD
{
	GENERATED_BODY()
};
