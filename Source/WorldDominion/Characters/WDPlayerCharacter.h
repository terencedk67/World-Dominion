// The Leader. Third-person, deliberate and powerful rather than a generic
// action-game soldier: no combat roll/cover system, just weighty movement,
// a premium shoulder camera, contextual interaction, vehicle entry and the
// Presidential View awareness overlay.
//
// Enhanced Input note: this class builds its UInputAction / UInputMappingContext
// objects natively in C++ (see SetupPlayerInputComponent) so the vertical slice
// is playable with zero .uasset Input Action authoring. A content team can later
// swap KeyMappings below for a designer-authored Input Mapping Context asset
// without touching the binding logic.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "WDPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class UWDInteractionComponent;
class UWDPresidentialViewComponent;
class UInputMappingContext;
class UInputAction;
class AActor;
enum class EWDVehicleSeatType : uint8;

UCLASS()
class WORLDDOMINION_API AWDPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AWDPlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Camera")
	TObjectPtr<UCameraComponent> FollowCamera;

	/** Visible stand-in body while no authored Skeletal Mesh / animations are assigned to GetMesh().
	 *  PLACEHOLDER: capsule primitive. FINAL ASSET REQUIRED: rigged leader Skeletal Mesh + anim Blueprint. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Placeholder")
	TObjectPtr<UStaticMeshComponent> PlaceholderBodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|Interaction")
	TObjectPtr<UWDInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WorldDominion|PresidentialView")
	TObjectPtr<UWDPresidentialViewComponent> PresidentialViewComponent;

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Vehicle")
	bool TryEnterFocusedVehicle();

	UFUNCTION(BlueprintCallable, Category = "WorldDominion|Vehicle")
	void ExitCurrentVehicle();

	UFUNCTION(BlueprintPure, Category = "WorldDominion|Vehicle")
	bool IsInVehicle() const { return CurrentVehicle.IsValid(); }

	/** Bound to the Phone input; UI module opens the presidential device widget. */
	UFUNCTION(BlueprintImplementableEvent, Category = "WorldDominion|UI")
	void OnOpenPresidentialDevice();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_SprintStart();
	void Input_SprintStop();
	void Input_Interact();
	void Input_TogglePresidentialView();
	void Input_EnterExitVehicle();
	void Input_OpenPhone();

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Movement")
	float WalkSpeed = 220.f;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Movement")
	float JogSpeed = 380.f;

	UPROPERTY(EditDefaultsOnly, Category = "WorldDominion|Movement")
	float SprintSpeed = 620.f;

	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> CurrentVehicle;

	EWDVehicleSeatType GetProtecteeSeatType() const;

private:
	// Natively-constructed Enhanced Input assets - see class comment.
	UPROPERTY(Transient)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> PresidentialViewAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> EnterExitVehicleAction;

	UPROPERTY(Transient)
	TObjectPtr<UInputAction> OpenPhoneAction;

	void BuildNativeInputAssets();
};
