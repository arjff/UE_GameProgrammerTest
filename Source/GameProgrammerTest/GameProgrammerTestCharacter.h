// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BlastPack.h"
#include "GameProgrammerTestCharacter.generated.h"


UCLASS(config=Game)
class AGameProgrammerTestCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Interact Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* InteractAction;

	/** Sprint Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	/** Throw Blast Pack Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ThrowBlastPackAction;


public:
	AGameProgrammerTestCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// Game Feature 1 - Interact with Door
	void InteractWithDoor();

	UPROPERTY(EditAnywhere, Category = "Door Interaction")
	float InteractionLineTraceLength = 75.f;

	// Game Feature 2 - Sprinting with Stamina System
	void Sprinting();

	void StopSprinting();

	UFUNCTION(BlueprintCallable)
	void CanIncreaseStamina(); // Allow stamina to be regained

	UFUNCTION(BlueprintCallable)
	void NoStamina(); // Force player to stop sprinting when stamina is depleted

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	float CurrentStamina = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	float MaxStamina = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	float StaminaRechargeRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	float StaminaConsumptionRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	float StaminaRechargeDelay = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sprinting")
	bool bCanStaminaRecharge = false;

	FTimerHandle StaminaRechargeTimerHandle;

	// Game Feature 3 - Double Jump
	UFUNCTION(BlueprintCallable)
	void DoubleJump();
	
	UPROPERTY()
	int JumpCount; 
	
	// Game Feature 4 - Blast Pack
	void ThrowBlastPack();

	UPROPERTY(EditAnywhere, Category = "Blast Pack")
	TSubclassOf<ABlastPack> BlastPackClass;
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

	// Reset JumpCount to 0
	virtual void Landed(const FHitResult& Hit) override; 

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

