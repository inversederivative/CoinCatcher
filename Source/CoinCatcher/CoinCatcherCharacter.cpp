// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoinCatcherCharacter.h"

#include "CC_CoinPickupActor.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "UMG.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ACoinCatcherCharacter

ACoinCatcherCharacter::ACoinCatcherCharacter()
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

	// Initialize Collected Coins Count
	CollectedCoins = 0;
}

void ACoinCatcherCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Call UpdateTotalCoins to count the coins at the beginning of the level
	UpdateTotalCoins();
	// Call once, to display the initial count of coins
	UpdateCoinCount();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ACoinCatcherCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ACoinCatcherCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACoinCatcherCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ACoinCatcherCharacter::Move(const FInputActionValue& Value)
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

void ACoinCatcherCharacter::Look(const FInputActionValue& Value)
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

int32 ACoinCatcherCharacter::GetCollectedCoins() const
{
	return CollectedCoins;
}

void ACoinCatcherCharacter::UpdateTotalCoins()
{
	TActorIterator<ACC_CoinPickupActor> CoinIterator(GetWorld());
	int32 TotalCoinCount = 0;

	while (CoinIterator)
	{
		TotalCoinCount++;
		++CoinIterator;
	}
	TotalCoins = TotalCoinCount;
	
	if (TotalCoins == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("The level has been spawned with NO COINS!"));
	}
}

void ACoinCatcherCharacter::CollectCoin()
{
	if (!IsCoinCollected)
	{
		CollectedCoins++;
		IsCoinCollected = true;
		UpdateCoinCount();

		IsCoinCollected = false;
	}
}



void ACoinCatcherCharacter::UpdateCoinCount()
{
	if (CollectedCoins >= TotalCoins)
	{
		// Trigger the win condition here (e.g., print a debug message)
		// FString YouWin = FString::Printf(TEXT("You did it! All Coins Collected!!!"));
		// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, YouWin);

		TriggerWin();
		
		UE_LOG(LogTemp, Warning, TEXT("You've collected all the coins! You win!"));
	}
	else
	{
		// Print the number of coins left to the screen as a debug message
		FString CoinsLeftMessage = FString::Printf(TEXT("Coins Left: %d / %d"), TotalCoins - CollectedCoins, TotalCoins);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, CoinsLeftMessage);
	}
}

void ACoinCatcherCharacter::TriggerWin()
{
	if (WinPopupWidgetClass)
	{
		UUserWidget* WinPopupWidget = CreateWidget<UUserWidget>(GetWorld(), WinPopupWidgetClass);
		if (WinPopupWidget)
		{
			WinPopupWidget->AddToViewport();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Win Popup Widget Class not Initialized Properly"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Win Popup Widget Class not Initialized Properly"));
	}
}

