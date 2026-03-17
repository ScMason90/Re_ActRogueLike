// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_TargetDummy.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Re_ActRogueLike/Components/AR_AttributeComponent.h"


// Sets default values
AAR_TargetDummy::AAR_TargetDummy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;
	
	AttributeComp = CreateDefaultSubobject<UAR_AttributeComponent>("AttributeComp");
	// Trigger when health is changed (damage/healing)
	AttributeComp->OnHealthChanged.AddDynamic(this, &AAR_TargetDummy::OnHealthChanged);
	
}

void AAR_TargetDummy::OnHealthChanged(
	AActor* InstigatorActor, UAR_AttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
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

