// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_HealthPotion.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"


// Sets default values
AAR_HealthPotion::AAR_HealthPotion()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	// Using 'SphereComp' in parent class for collision query instead of subclass.Disable it.
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAR_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	Super::Interact_Implementation(InstigatorPawn);
	
	if (!IsValid(InstigatorPawn))return;
	
	UAR_AttributeComponent* AttributeComp = Cast<UAR_AttributeComponent>(
		InstigatorPawn->GetComponentByClass(UAR_AttributeComponent::StaticClass()));
	// Check if not at max health
	if (IsValid(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		// Only activate if healed successfully
		if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetMaxHealth()))
		{
			HideAndCooldownPickup();
		}
	}
}


