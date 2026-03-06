// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Re_ActRogueLike/Core/AR_GameplayInterface.h"
#include "AR_ItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class RE_ACTROGUELIKE_API AAR_ItemChest : public AActor, public IAR_GameplayInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_ItemChest();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BaseMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;
	
	UPROPERTY(EditAnywhere, Category = "OpenChest")
	float TargetPitch;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
