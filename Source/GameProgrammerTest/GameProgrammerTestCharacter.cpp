// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameProgrammerTestCharacter.h"
#include "EngineUtils.h"
#include "Door.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AGameProgrammerTestCharacter

AGameProgrammerTestCharacter::AGameProgrammerTestCharacter()
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

}

void AGameProgrammerTestCharacter::BeginPlay()
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

}

//////////////////////////////////////////////////////////////////////////
// Input

void AGameProgrammerTestCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Door Interaction
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AGameProgrammerTestCharacter::InteractWithDoor);
		
		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AGameProgrammerTestCharacter::Sprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AGameProgrammerTestCharacter::StopSprinting);
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGameProgrammerTestCharacter::DoubleJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Throwing Blast Pack
		EnhancedInputComponent->BindAction(ThrowBlastPackAction, ETriggerEvent::Started, this, &AGameProgrammerTestCharacter::ThrowBlastPack);


		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGameProgrammerTestCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGameProgrammerTestCharacter::Look);

	}

}

void AGameProgrammerTestCharacter::Move(const FInputActionValue& Value)
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

void AGameProgrammerTestCharacter::Look(const FInputActionValue& Value)
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

// Game Feature 1 - Interact With Door
void AGameProgrammerTestCharacter::InteractWithDoor()
{
	if(GetCapsuleComponent() == nullptr)
		return;

	FHitResult HitResult;

	FVector Start = GetCapsuleComponent()->GetComponentLocation();
	FVector End = Start + GetCapsuleComponent()->GetForwardVector() * InteractionLineTraceLength;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
	DrawDebugLine(GetWorld(),Start,End, FColor::Red, false, 2.f);

	// Checks if Line Trace hits the Door Actor
	ADoor* Door = Cast<ADoor>(HitResult.GetActor());
	if(Door)
	{
		Door->GamePlayer = this;
		Door->OnInteract();
	}
}


// Game Feature 2 - Sprinting with Stamina
void AGameProgrammerTestCharacter::Sprinting()
{
	// Check if Player has Stamina and if Player is Moving
	if((CurrentStamina > 0.f) && (GetCharacterMovement()->Velocity != FVector::ZeroVector)) 
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = 1200.f;
		// UE_LOG(LogTemp,Warning, TEXT("Sprinting"));

		bCanStaminaRecharge = false;
		GetWorld()->GetTimerManager().ClearTimer(StaminaRechargeTimerHandle);
	}
}

void AGameProgrammerTestCharacter::StopSprinting()
{
	if(bIsSprinting) // to not get into a loop of continuous sprinting
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		// UE_LOG(LogTemp,Warning, TEXT("Stopped sprinting"));

		GetWorld()->GetTimerManager().SetTimer(StaminaRechargeTimerHandle, this, &AGameProgrammerTestCharacter::CanIncreaseStamina, StaminaRechargeDelay, false);
	}
}

void AGameProgrammerTestCharacter::CanIncreaseStamina()
{
	bCanStaminaRecharge = true;
}

void AGameProgrammerTestCharacter::NoStamina()
{
	StopSprinting();
}


// Game Feature 3 - Double Jump
void AGameProgrammerTestCharacter::DoubleJump()
{
	if(JumpCount < 2)
	{
		LaunchCharacter(FVector(0.f,0.f,500.f),false,true);
		JumpCount++;
	}
}

void AGameProgrammerTestCharacter::Landed(const FHitResult &Hit)
{
	JumpCount = 0;
}


// Game Feature 4 - Throw Blast Pack
void AGameProgrammerTestCharacter::ThrowBlastPack()
{
	for(ABlastPack* BlastPackSpawned : TActorRange<ABlastPack>(GetWorld()))
	{
		BlastPackSpawned->Detonate();
		return;
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("Spawn"));
	const FRotator ActorRotation =  GetCapsuleComponent()->GetComponentRotation();
	const FVector ActorLocation = (GetCapsuleComponent()->GetComponentLocation()) + ((GetCapsuleComponent()->GetForwardVector())*50.f);
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// Spawn Blast Pack
	GetWorld()->SpawnActor<ABlastPack>(BlastPackClass, ActorLocation, ActorRotation);
}


void AGameProgrammerTestCharacter::Tick(float DeltaTime)
{
	if(bIsSprinting)
	{
		CurrentStamina = FMath::FInterpConstantTo(CurrentStamina, 0.f, DeltaTime, StaminaConsumptionRate);

		if(CurrentStamina <= 0.0f)
		{
			NoStamina();
		}
	}
	else
	{
		if(CurrentStamina < MaxStamina)
		{
			if(bCanStaminaRecharge)
			{
				CurrentStamina = FMath::FInterpConstantTo(CurrentStamina,MaxStamina,DeltaTime,StaminaRechargeRate);
			}
			
		}
	}
}

