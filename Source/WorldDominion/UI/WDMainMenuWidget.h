// C++ backing for the main menu (WORLD DOMINION logo -> New Game / Continue /
// Settings, per the key art). Deliberately thin: it only knows how to move
// the player to Nation Select or resume a save; the premium gold/navy layout
// itself is authored in a WBP_MainMenu Blueprint child in-editor.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WDMainMenuWidget.generated.h"

UCLASS()
class WORLDDOMINION_API UWDMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Soft level reference to the Nation Select screen level/sub-level. */
	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|MainMenu")
	TSoftObjectPtr<UWorld> NationSelectLevel;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|MainMenu")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "WorldDominion|MainMenu")
	bool HasSaveGame() const;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|MainMenu")
	void ContinueGame();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|MainMenu")
	void QuitGame();
};
