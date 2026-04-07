// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TargetDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"


// Sets default values
AAR_TargetDummy::AAR_TargetDummy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComponent;
	
	ActionSystemComponent = CreateDefaultSubobject<UAR_ActionSystemComponent>("ActionSystemComp");
	// Trigger when health is changed (damage/healing)
	ActionSystemComponent->OnHealthChanged.AddDynamic(this, &AAR_TargetDummy::OnHealthChanged);
	
}

void AAR_TargetDummy::OnHealthChanged(
	AActor* InstigatorActor, UAR_ActionSystemComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComponent->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
	}
}

// Called when the game starts or when spawned
void AAR_TargetDummy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAR_TargetDummy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float AAR_TargetDummy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	ActionSystemComponent->ApplyHealthChange(DamageCauser, -DamageAmount);
	
	return ActualDamage;
}

