// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ItemChest.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

void AAR_ItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	IAR_InteractionInterface::Interact_Implementation(InstigatorPawn);
	
	LidMesh->SetRelativeRotation(FQuat(FVector::RightVector, FMath::DegreesToRadians(TargetPitch)));
}

// Sets default values
AAR_ItemChest::AAR_ItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled =false;
	
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);
	LidMesh->SetSimulatePhysics(false);
	
	TargetPitch = -110.0f;
}

// Called when the game starts or when spawned
void AAR_ItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// void ARogueItemChest::Interact_Implementation()
// {
// 	// Play Animation
// 	SetActorTickEnabled(true);
// }

// Called every frame
void AAR_ItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//
	// CurrentAnimationPitch = FMath::FInterpConstantTo(CurrentAnimationPitch, AnimationTargetPitch, DeltaTime, AnimationSpeed);
	// LidMeshComponent->SetRelativeRotation(FRotator(CurrentAnimationPitch, 0.0f, 0.0f));
	//
	// if (FMath::IsNearlyEqual(CurrentAnimationPitch, AnimationTargetPitch))
	// {
	// 	// Animation Complete
	// 	SetActorTickEnabled(false);
	//
	// 	ChestAnimationComplete();
	// }
}

