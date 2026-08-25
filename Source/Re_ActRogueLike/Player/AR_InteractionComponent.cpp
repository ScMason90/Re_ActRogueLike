// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_InteractionComponent.h"

#include "Engine/OverlapResult.h"
#include "Re_ActRogueLike/Core/AR_InteractionInterface.h"
#include "Re_ActRogueLike/Re_ActRoguelikeTypes.h"

static TAutoConsoleVariable<bool> CVarInteractionDebugDrawing(TEXT("game.interaction.DebugDraw"), false,
	TEXT("Enable interaction component debug rendering. (0 = off, 1 = enabled)"), ECVF_Cheat);

UAR_InteractionComponent::UAR_InteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}

void UAR_InteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// UE_LOG(LogGame, Warning, TEXT("UAR_InteractionComponent::TickComponent::Owner = %s"), *GetOwner()->GetName());
	
	APlayerController* PC = CastChecked<APlayerController>(GetOwner());
	
	FVector OwnerCenter = GetOwner()->GetActorLocation();
	FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
	
	ECollisionChannel CollisionChannel = COLLISION_INTERACTION;
	
	FCollisionShape Shape;
	Shape.SetSphere(InteractionRadius);
	
	float InteractionRadiusSqrd = pow(InteractionRadius, 2);
	
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByChannel(Overlaps, OwnerCenter, FQuat::Identity, CollisionChannel, Shape);
	
	bool bEnabledDebugDraw = CVarInteractionDebugDrawing.GetValueOnGameThread();
	
	AActor* BestActor = nullptr;
	float HighestWeight = 0.0f;
	
	for (FOverlapResult& Overlap : Overlaps)
	{
		AActor* OverlappedActor = Overlap.GetActor();	// temp for each loop...
		FVector Origin/*Center of overlapped actor*/, BoxExtends/*Half size of ... in 3d space*/;
		OverlappedActor->GetActorBounds(true, Origin, BoxExtends);
		
		// Direction vector from Camera to object(overlapped in current iter-variable Overlap_
		FVector OverlapDirection = (Origin - CameraLocation).GetSafeNormal();
		
		float DistanceToSqrd = (Origin - OwnerCenter).SizeSquared();	// Dist between player and object
		// Normalize and invert, smaller dist(this variable) is higher weight
		float NormalizedDistanceTo = 1.0f - (DistanceToSqrd / InteractionRadiusSqrd);
		
		float DotResult = FVector::DotProduct(OverlapDirection, PC->GetControlRotation().Vector());
		// Normalize DotResult from [-1,1] to [0,1]
		float NormalizedDotResult = DotResult * 0.5f +0.5f;
		
		float Weight = (NormalizedDotResult * DirectionWeightScale) + (NormalizedDistanceTo * DistanceToWeightScale);
		
		if (Weight > HighestWeight)
		{
			HighestWeight = Weight;
			BestActor = OverlappedActor;
		}
		
		if (bEnabledDebugDraw)
		{
			DrawDebugBox(GetWorld(), Origin, FVector(50.0f), FColor::Red);
			FString DebugString = FString::Printf(TEXT("Weight: %f, Dot: %f, Dist: %f"), Weight, NormalizedDotResult, NormalizedDistanceTo);
			DrawDebugString(GetWorld(), Origin, DebugString, nullptr, FColor::White, 0.0f, true);
		}
	}
	
	
	SelectedActor = BestActor;
	if (bEnabledDebugDraw)
	{
		if (IsValid(BestActor))
		{
			DrawDebugBox(GetWorld(), BestActor->GetActorLocation(), FVector(60.0f), FColor::Green);
		}
		
		DrawDebugSphere(GetWorld(), OwnerCenter, InteractionRadius, 32,FColor::White);
	}
}

void UAR_InteractionComponent::PrimaryInteraction()
{
	if (IsValid(SelectedActor) && SelectedActor->Implements<UAR_InteractionInterface>())
	{
		APawn* MyPawn = Cast<APawn>(GetOwner());
		IAR_InteractionInterface::Execute_Interact(SelectedActor, MyPawn);
	}
}