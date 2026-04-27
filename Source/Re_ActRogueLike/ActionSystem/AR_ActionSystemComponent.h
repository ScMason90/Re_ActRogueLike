// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AR_ActionSystemComponent.generated.h"

class UAR_ActionSystem;

USTRUCT(BlueprintType)
struct FAR_AttributeSet
{
	GENERATED_BODY()
	
	FAR_AttributeSet():Health(100.0f),MaxHealth(100.0f){}
	
	UPROPERTY(BlueprintReadOnly)
	float Health;
	
	UPROPERTY(BlueprintReadOnly)
	float MaxHealth;
	
	// Stamina, Strength
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FOnHealthChanged, AActor*, Instigator, UAR_ActionSystemComponent*, OwingComp, float, NewHealth, float, Delta);

/*
 *
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RE_ACTROGUELIKE_API UAR_ActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAR_ActionSystemComponent();

protected:
	// EditAnywhere - edit in BP editor and per-instance in level.
	// VisibleAnywhere - 'read-only' in editor and level. (Use for Components)
	// EditDefaultsOnly - hide variable per-instance, edit in BP editor only
	// VisibleDefaultsOnly - 'read-only' access for variable, only in BP editor (uncommon)
	// EditInstanceOnly - allow only editing of instance (e.g. when placed in level)
	// --
	// BlueprintReadOnly - read-only in the Blueprint scripting (does not affect 'details'-panel)
	// BlueprintReadWrite - read-write access in Blueprints
	// --
	// Category = "" - display only for detail panels and blueprint context menu.
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Attributes")
	FAR_AttributeSet Attributes;
	
	UPROPERTY()
	TArray<TObjectPtr<UAR_ActionSystem>> Actions;

public:
	virtual void InitializeComponent() override;
	
	/*--------------- Attributes Relative ------------------*/
	/* Health */
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Setters")
	bool ApplyHealthChange(AActor* Instigator, float Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	bool IsDead() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	float GetMaxHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	bool IsFullHealth() const;
	
	/*---------------------- Actions Relative ---------------------*/
	void StartAction(FName InActionName);
};
