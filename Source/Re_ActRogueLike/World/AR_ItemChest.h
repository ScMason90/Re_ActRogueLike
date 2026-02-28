// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_ItemChest.generated.h"

class UStaticMeshComponent;
UCLASS()
class RE_ACTROGUELIKE_API AAR_ItemChest : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_ItemChest();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LidMesh;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
