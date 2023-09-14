// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

UCLASS()
class GAMEPROGRAMMERTEST_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OpenDoor(float Value); 

	UFUNCTION()
	void SetDoorOnSameSide();

	UPROPERTY(EditAnywhere)
	UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere)
	bool bIsDoorClosed = true;

	UPROPERTY(EditAnywhere)
	float DoorRotateAngle = 90.f;

	UPROPERTY(EditAnywhere)
	bool bDoorOnSameSide;
	
	FTimeline Timeline;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnInteract();

	class AGameProgrammerTestCharacter* GamePlayer;

private:

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* DoorFrame;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* Door;

	UPROPERTY(VisibleAnywhere, Category = "Collision Box")
	UBoxComponent* CollisionBox;

};
