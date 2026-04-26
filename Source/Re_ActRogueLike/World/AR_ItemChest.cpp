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