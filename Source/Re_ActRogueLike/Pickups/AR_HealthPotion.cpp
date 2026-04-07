// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_HealthPotion.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"


// Sets default values
AAR_HealthPotion::AAR_HealthPotion()
{
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	PickupMeshComponent->SetupAttachment(RootComponent);
	// Using 'SphereComp' in parent class for collision query instead of subclass.Disable it.
	PickupMeshComponent->SetCollisionProfileName("NoCollision");
	PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Set collision profile(object type), make this in Project Settings, "Overlap" only to Pawn
	OverlapComponent->SetCollisionProfileName("Pickups");
	OverlapComponent->SetupAttachment(PickupMeshComponent);
}

void AAR_HealthPotion::OnActorOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnActorOverlapped(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	UAR_ActionSystemComponent* AttributeComp = OtherActor->GetComponentByClass<UAR_ActionSystemComponent>();
	
	// Assert if null, then we misconfigured what we can overlap with, any Pawn should have an action component
	// Skip Health Potion pickup if already full health
	if (IsValid(AttributeComp)/*ensure(AttributeComp != nullptr)*/ && !AttributeComp->IsFullHealth())
	{
		// TODO: Considering add heal up material flash VFX...or SFX for both damaged and healed?
		AttributeComp->ApplyHealthChange(this, HealingAmount);
		
		// Play(valid context and location) before destroying actor
		UGameplayStatics::PlaySoundAtLocation(
			this, PickupSound, GetActorLocation(), FRotator::ZeroRotator);
		
		// Remove Actor from world, eventually memory will be freed (garbage collection)
		Destroy();
	}
}