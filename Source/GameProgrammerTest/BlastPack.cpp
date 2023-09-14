// Fill out your copyright notice in the Description page of Project Settings.


#include "BlastPack.h"

// Sets default values
ABlastPack::ABlastPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlastPack = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Blast Pack"));
	RootComponent = BlastPack;
	RadialForce  = CreateDefaultSubobject<URadialForceComponent>(TEXT("Radial Force"));
	RadialForce->SetupAttachment(BlastPack);
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	
	BlastPack->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BlastPack->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	BlastPack->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	RadialForce->Radius = 600.f;
	RadialForce->ImpulseStrength = 100000.f;

	ProjectileMovement->InitialSpeed = 600.f;
	ProjectileMovement->MaxSpeed = 0.0f;
}

// Called when the game starts or when spawned
void ABlastPack::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlastPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlastPack::Detonate()
{
	RadialForce->FireImpulse();
	Destroy();
	
}

