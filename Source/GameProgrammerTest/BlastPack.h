// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BlastPack.generated.h"

UCLASS()
class GAMEPROGRAMMERTEST_API ABlastPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlastPack();

	UFUNCTION()
	void Detonate();

private:
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* BlastPack;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class URadialForceComponent* RadialForce;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	class UProjectileMovementComponent* ProjectileMovement;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
