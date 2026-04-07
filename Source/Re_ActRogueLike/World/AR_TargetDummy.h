// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AR_TargetDummy.generated.h"

class UAR_ActionSystemComponent;

UCLASS()
class RE_ACTROGUELIKE_API AAR_TargetDummy : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAR_TargetDummy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAR_ActionSystemComponent> ActionSystemComponent;
	
	UFUNCTION()
	void OnHealthChanged(
		AActor* InstigatorActor, UAR_ActionSystemComponent* OwningComp, float NewHealth, float Delta);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};
