// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_InteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Re_ActRogueLike/Core/AR_GameplayInterface.h"


// Sets default values for this component's properties
UAR_InteractionComponent::UAR_InteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAR_InteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAR_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAR_InteractionComponent::PrimaryInteraction()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	AActor* MyOwner = GetOwner();
	
	FVector EyeLocation, End;
	FRotator EyeRotation;
	
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	End = EyeLocation + (EyeRotation.Vector() * 200.0f);
	
	// FHitResult Hit;
	// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);
	
	float Radius = 30.0f;
	
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	
	TArray<FHitResult> Hits;
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End,
		FQuat::Identity, ObjectQueryParams, Shape);
	
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	
	for (FHitResult Hit : Hits)
	{
		DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, 
			LineColor, false, 2.0f);
		
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor->Implements<UAR_GameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
			
				IAR_GameplayInterface::Execute_Interact(HitActor, MyPawn);
				break;
			}
		}
	}
	
	DrawDebugLine(GetWorld(), EyeLocation, End, LineColor,
		false, 2.0f, 0, 2.0f);
}

