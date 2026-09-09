// The President/Leader's controller. Persists across pawn possession changes
// (on-foot -> vehicle -> aircraft), so it owns the entourage/security system
// and the HUD widget rather than the pawn.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WDPlayerController.generated.h"

class UWDPresidentialSecurityComponent;
class UWDHUDWidget;
class UInputMappingContext;

UCLASS()
class WORLDDOMINION_API AWDPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWDPlayerController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Security")
	TObjectPtr<UWDPresidentialSecurityComponent> PresidentialSecurity;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|UI")
	UWDHUDWidget* GetHUDWidget() const { return HUDWidget; }

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/** Widget class assignable from a Blueprint child (WBP_HUD) once the UMG layout exists. */
	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|UI")
	TSubclassOf<UWDHUDWidget> HUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UWDHUDWidget> HUDWidget;
};
