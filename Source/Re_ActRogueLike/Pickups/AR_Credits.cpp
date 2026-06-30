// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_Credits.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Player/AR_PlayerState.h"


// Sets default values
AAR_Credits::AAR_Credits()
{
	// Using 'SphereComp' in parent class for collision query instead of subclass...
	
	// Set collision profile(object type), make this in Project Settings, "Overlap" only to Pawn
	OverlapComponent->SetCollisionProfileName("Pickups");
	
	CreditsAmount = 50;
}

void AAR_Credits::OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnActorOverlapped(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	if (APawn* OtherPawn = Cast<APawn>(OtherActor);AAR_PlayerState* PS = OtherPawn->GetPlayerState<AAR_PlayerState>())
	{
		PS->AddCredits(CreditsAmount);
		
		UGameplayStatics::PlaySoundAtLocation(
			this, PickupSound, GetActorLocation(), FRotator::ZeroRotator);
		
		Destroy();
	}
}

