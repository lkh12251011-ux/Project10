// Copyright Epic Games, Inc. All Rights Reserved.

#include "Project10Character.h"
#include "Test/TestActor.h" 
#include "Test/IndianPokerGameMode.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AProject10Character

AProject10Character::AProject10Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	HeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HeadWidget"));
	HeadWidgetComponent->SetupAttachment(GetMesh(), FName("head"));
	HeadWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HeadWidgetComponent->SetDrawSize(FVector2D(80.f, 80.f));
	HeadWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
}

void AProject10Character::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters Params;
	GetWorld()->SpawnActor<ATestActor>(
		ATestActor::StaticClass(),
		GetActorTransform(),
		Params
	);

	AIndianPokerGameMode* GM =
		Cast<AIndianPokerGameMode>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		
		GM->SetPlayerAction(0, EPlayerAction::Attack);
		GM->SetPlayerAction(1, EPlayerAction::Defend);
	}
}
//////////////////////////////////////////////////////////////////////////
// Input

void AProject10Character::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AProject10Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProject10Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProject10Character::Look);

		if (ActionModeAction)
		{
			EnhancedInputComponent->BindAction(ActionModeAction, ETriggerEvent::Started,
				this, &AProject10Character::ToggleActionMode);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AProject10Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AProject10Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AProject10Character::UpdateHeadNumber(int32 Number)
{
	if (!HeadWidgetComponent) return;

	UUserWidget* Widget = HeadWidgetComponent->GetUserWidgetObject();
	if (!Widget) return;

	UTextBlock* Text = Cast<UTextBlock>(
		Widget->GetWidgetFromName(TEXT("HeadNumberText")));

	if (Text)
	{
		if (Number < 0)
			Text->SetText(FText::FromString(TEXT("?")));
		else
			Text->SetText(FText::AsNumber(Number));
	}
}

void AProject10Character::ToggleActionMode()
{
	bInActionMode = !bInActionMode;

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) return;

	if (bInActionMode)
	{
		PC->SetShowMouseCursor(true);
		GetCharacterMovement()->DisableMovement();
		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(10, 3.f, FColor::Yellow,
				TEXT("행동 선택 모드 ON (E키로 끄기)"));
	}
	else
	{
		PC->SetShowMouseCursor(false);
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		PC->SetInputMode(FInputModeGameOnly());

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(10, 3.f, FColor::Green,
				TEXT("행동 선택 모드 OFF"));
	}
}