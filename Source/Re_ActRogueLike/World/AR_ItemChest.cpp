// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ItemChest.h"

#include "Components/StaticMeshComponent.h"

void AAR_ItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	IAR_GameplayInterface::Interact_Implementation(InstigatorPawn);
	
	bIsOpening = true;
}

// Sets default values
AAR_ItemChest::AAR_ItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);
	LidMesh->SetSimulatePhysics(false);
}

// Called when the game starts or when spawned
void AAR_ItemChest::BeginPlay()
{
	Super::BeginPlay();
	
	TargetPitch = 110.0f;
	StaticRotation = LidMesh->GetRelativeRotation();
	TargetRotation = StaticRotation + FRotator(TargetPitch, 0.0f, 0.0f);
	
	StaticRotation.Normalize();
	TargetRotation.Normalize();
}

// Called every frame
void AAR_ItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsOpening) return;

	// Get current&target quaternion via FRotator
	FQuat CurrentQ = LidMesh->GetRelativeRotation().Quaternion();
	FQuat TargetQ  = TargetRotation.Quaternion();

	// Compute Lerp factor(ensure it in [0,1])
	// Simple linear: alpha = clamp(OpenSpeed * DeltaTime, 0, 1)
	// Also more smooth: alpha = 1 - exp(-OpenSpeed * DeltaTime)
	float Alpha = FMath::Clamp(OpenSpeed * DeltaTime, 0.0f, 1.0f);

	// Spherical Linear Interpolation(Slerp)
	FQuat NewQ = FQuat::Slerp(CurrentQ, TargetQ, Alpha);
	NewQ.Normalize();

	// Apply rotation
	LidMesh->SetRelativeRotation(NewQ);

	// Final determination: Convert NewQ and TargetQ to Rotator to compare the Angle tolerance (more intuitive)
	FRotator NewRot = NewQ.Rotator();
	if (NewRot.Equals(TargetRotation, 0.5f)) 
	{
		// Force alignment to the precise target to avoid residual jitter
		LidMesh->SetRelativeRotation(TargetRotation);
		bIsOpening = false;
		
		// TODO: open-close-open loop.
		TargetPitch = -TargetPitch;
		TargetRotation = FRotator(TargetPitch, 0.0f, 0.0f);
	}
}

