#include "WDPlayerCharacter.h"
#include "WorldDominion.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "WDInteractionComponent.h"
#include "WDPresidentialViewComponent.h"
#include "IWDVehicleInteractable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputModifiers.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerController.h"
#include "WDCharacterVisuals.h"

AWDPlayerCharacter::AWDPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 1400.f;
	GetCharacterMovement()->JumpZVelocity = 0.f; // A head of state does not jump.
	GetCharacterMovement()->AirControl = 0.f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 320.f;
	CameraBoom->SocketOffset = FVector(0.f, 65.f, 60.f); // premium over-the-shoulder offset
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.f;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 10.f;
	CameraBoom->bDoCollisionTest = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetFieldOfView(50.f); // cinematic, not wide/arcade

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaceholderMeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	PlaceholderBodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderBodyMesh"));
	PlaceholderBodyMesh->SetupAttachment(RootComponent);
	if (PlaceholderMeshFinder.Succeeded())
	{
		PlaceholderBodyMesh->SetStaticMesh(PlaceholderMeshFinder.Object);
	}
	PlaceholderBodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
	PlaceholderBodyMesh->SetRelativeScale3D(FVector(0.55f, 0.55f, 1.75f));
	PlaceholderBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlaceholderBodyMesh->SetCastShadow(true);
	GetMesh()->SetVisibility(false); // hide the empty default skeletal mesh component until a real one is assigned

	InteractionComponent = CreateDefaultSubobject<UWDInteractionComponent>(TEXT("InteractionComponent"));
	PresidentialViewComponent = CreateDefaultSubobject<UWDPresidentialViewComponent>(TEXT("PresidentialViewComponent"));
}

void AWDPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	BuildNativeInputAssets();
	WDCharacterVisuals::ApplyPlaceholderSwap(GetMesh(), PlaceholderBodyMesh);
}

void AWDPlayerCharacter::BuildNativeInputAssets()
{
	DefaultMappingContext = NewObject<UInputMappingContext>(this, TEXT("IMC_WDDefault"));

	auto MakeAction = [this](FName Name, EInputActionValueType ValueType) -> UInputAction*
	{
		UInputAction* Action = NewObject<UInputAction>(this, Name);
		Action->ValueType = ValueType;
		return Action;
	};

	MoveAction = MakeAction(TEXT("IA_Move"), EInputActionValueType::Axis2D);
	LookAction = MakeAction(TEXT("IA_Look"), EInputActionValueType::Axis2D);
	SprintAction = MakeAction(TEXT("IA_Sprint"), EInputActionValueType::Boolean);
	InteractAction = MakeAction(TEXT("IA_Interact"), EInputActionValueType::Boolean);
	PresidentialViewAction = MakeAction(TEXT("IA_PresidentialView"), EInputActionValueType::Boolean);
	EnterExitVehicleAction = MakeAction(TEXT("IA_EnterExitVehicle"), EInputActionValueType::Boolean);
	OpenPhoneAction = MakeAction(TEXT("IA_OpenPhone"), EInputActionValueType::Boolean);

	// WASD -> IA_Move (Axis2D). Mirrors the modifier stack Epic's own ThirdPerson
	// template ships in IMC_Default, just built natively instead of authored as a
	// .uasset: W/S swizzle the digital 1 from the key's X slot into the move
	// action's Y (forward) slot, A/D stay on X (strafe), and S/A negate.
	UInputModifierSwizzleAxis* SwizzleForward = NewObject<UInputModifierSwizzleAxis>(this);
	SwizzleForward->Order = EInputAxisSwizzle::YXZ;
	UInputModifierSwizzleAxis* SwizzleBack = NewObject<UInputModifierSwizzleAxis>(this);
	SwizzleBack->Order = EInputAxisSwizzle::YXZ;
	UInputModifierNegate* NegateBack = NewObject<UInputModifierNegate>(this);
	UInputModifierNegate* NegateLeft = NewObject<UInputModifierNegate>(this);

	DefaultMappingContext->MapKey(MoveAction, EKeys::W).Modifiers.Add(SwizzleForward);
	FEnhancedActionKeyMapping& S_Mapping = DefaultMappingContext->MapKey(MoveAction, EKeys::S);
	S_Mapping.Modifiers.Add(SwizzleBack);
	S_Mapping.Modifiers.Add(NegateBack);
	DefaultMappingContext->MapKey(MoveAction, EKeys::A).Modifiers.Add(NegateLeft);
	DefaultMappingContext->MapKey(MoveAction, EKeys::D);

	UInputModifierSwizzleAxis* SwizzleGamepadMoveY = NewObject<UInputModifierSwizzleAxis>(this);
	SwizzleGamepadMoveY->Order = EInputAxisSwizzle::YXZ;
	DefaultMappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftY).Modifiers.Add(SwizzleGamepadMoveY);
	DefaultMappingContext->MapKey(MoveAction, EKeys::Gamepad_LeftX);

	// Look -> IA_Look (Axis2D). Mouse2D is a genuine Vector2D-valued Enhanced Input key
	// (raw mouse delta), so it maps directly with no modifiers needed.
	DefaultMappingContext->MapKey(LookAction, EKeys::Mouse2D);

	UInputModifierSwizzleAxis* SwizzleGamepadLookY = NewObject<UInputModifierSwizzleAxis>(this);
	SwizzleGamepadLookY->Order = EInputAxisSwizzle::YXZ;
	UInputModifierNegate* NegateLookY = NewObject<UInputModifierNegate>(this);
	FEnhancedActionKeyMapping& LookY_Mapping = DefaultMappingContext->MapKey(LookAction, EKeys::Gamepad_RightY);
	LookY_Mapping.Modifiers.Add(SwizzleGamepadLookY);
	LookY_Mapping.Modifiers.Add(NegateLookY);
	DefaultMappingContext->MapKey(LookAction, EKeys::Gamepad_RightX);

	DefaultMappingContext->MapKey(SprintAction, EKeys::LeftShift);
	DefaultMappingContext->MapKey(SprintAction, EKeys::Gamepad_LeftTrigger);
	DefaultMappingContext->MapKey(InteractAction, EKeys::Y);
	DefaultMappingContext->MapKey(InteractAction, EKeys::Gamepad_FaceButton_Top);
	DefaultMappingContext->MapKey(PresidentialViewAction, EKeys::RightBracket);
	DefaultMappingContext->MapKey(PresidentialViewAction, EKeys::Gamepad_RightThumbstick);
	DefaultMappingContext->MapKey(EnterExitVehicleAction, EKeys::E);
	DefaultMappingContext->MapKey(EnterExitVehicleAction, EKeys::Gamepad_FaceButton_Left);
	DefaultMappingContext->MapKey(OpenPhoneAction, EKeys::Tab);
	DefaultMappingContext->MapKey(OpenPhoneAction, EKeys::Gamepad_Special_Right);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AWDPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWDPlayerCharacter::Input_Move);
		EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWDPlayerCharacter::Input_Look);
		EIC->BindAction(SprintAction, ETriggerEvent::Started, this, &AWDPlayerCharacter::Input_SprintStart);
		EIC->BindAction(SprintAction, ETriggerEvent::Completed, this, &AWDPlayerCharacter::Input_SprintStop);
		EIC->BindAction(SprintAction, ETriggerEvent::Canceled, this, &AWDPlayerCharacter::Input_SprintStop);
		EIC->BindAction(InteractAction, ETriggerEvent::Started, this, &AWDPlayerCharacter::Input_Interact);
		EIC->BindAction(PresidentialViewAction, ETriggerEvent::Started, this, &AWDPlayerCharacter::Input_TogglePresidentialView);
		EIC->BindAction(EnterExitVehicleAction, ETriggerEvent::Started, this, &AWDPlayerCharacter::Input_EnterExitVehicle);
		EIC->BindAction(OpenPhoneAction, ETriggerEvent::Started, this, &AWDPlayerCharacter::Input_OpenPhone);
	}
	else
	{
		UE_LOG(LogWorldDominion, Error, TEXT("AWDPlayerCharacter requires an EnhancedInputComponent. Check DefaultEngine.ini [/Script/Engine.InputSettings]."));
	}
}

void AWDPlayerCharacter::Input_Move(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (!Controller || Axis.IsNearlyZero())
	{
		return;
	}

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Axis.X);
}

void AWDPlayerCharacter::Input_Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(-Axis.Y);
}

void AWDPlayerCharacter::Input_SprintStart()
{
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void AWDPlayerCharacter::Input_SprintStop()
{
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
}

void AWDPlayerCharacter::Input_Interact()
{
	if (InteractionComponent)
	{
		InteractionComponent->TryInteract();
	}
}

void AWDPlayerCharacter::Input_TogglePresidentialView()
{
	if (PresidentialViewComponent)
	{
		PresidentialViewComponent->ToggleActive();
	}
}

void AWDPlayerCharacter::Input_EnterExitVehicle()
{
	if (IsInVehicle())
	{
		ExitCurrentVehicle();
	}
	else
	{
		TryEnterFocusedVehicle();
	}
}

void AWDPlayerCharacter::Input_OpenPhone()
{
	OnOpenPresidentialDevice();
}

EWDVehicleSeatType AWDPlayerCharacter::GetProtecteeSeatType() const
{
	return EWDVehicleSeatType::Protectee;
}

bool AWDPlayerCharacter::TryEnterFocusedVehicle()
{
	AActor* Focus = InteractionComponent ? InteractionComponent->GetFocusedInteractable() : nullptr;
	if (Focus && Focus->Implements<UWDVehicleInteractable>())
	{
		if (IWDVehicleInteractable::Execute_EnterVehicle(Focus, this, GetProtecteeSeatType()))
		{
			CurrentVehicle = Focus;
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			return true;
		}
	}
	return false;
}

void AWDPlayerCharacter::ExitCurrentVehicle()
{
	if (AActor* Vehicle = CurrentVehicle.Get())
	{
		if (Vehicle->Implements<UWDVehicleInteractable>())
		{
			IWDVehicleInteractable::Execute_ExitVehicle(Vehicle, this);
		}
	}
	CurrentVehicle.Reset();
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}
