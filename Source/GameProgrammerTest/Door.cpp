// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "GameProgrammerTestCharacter.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorFrame"));
	DoorFrame->SetupAttachment(RootComponent);

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(DoorFrame);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(Door);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();

	if(CurveFloat)
	{
		FOnTimelineFloat TimelineProgress;
		TimelineProgress.BindDynamic(this, &ADoor::OpenDoor);
		Timeline.AddInterpFloat(CurveFloat, TimelineProgress);
	}
	
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timeline.TickTimeline(DeltaTime);

}

void ADoor::OnInteract() 
{
	if(bIsDoorClosed)
	{
		SetDoorOnSameSide();
		Timeline.Play();
		//UE_LOG(LogTemp, Display, TEXT("Opened"));
	}
	else
	{
		Timeline.Reverse();
		//UE_LOG(LogTemp, Display, TEXT("Closed"));
	}

	bIsDoorClosed = !bIsDoorClosed;
}

void ADoor::OpenDoor(float Value)
{
	float Angle = bDoorOnSameSide ? DoorRotateAngle: -DoorRotateAngle;

	FRotator Rot = FRotator(0.f, Angle * Value, 0.f);

	Door->SetRelativeRotation(Rot);
}

void ADoor::SetDoorOnSameSide() // Ensuring the door opens away from the player
{
	if(GamePlayer)
	{
		FVector CharacterFrontVec = GamePlayer->GetActorForwardVector();
		FVector DoorFrontVec = GetActorForwardVector();
		bDoorOnSameSide = FVector::DotProduct(CharacterFrontVec, DoorFrontVec) >= 0;
	}
}

