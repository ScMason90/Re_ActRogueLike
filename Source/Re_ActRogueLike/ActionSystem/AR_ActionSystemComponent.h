// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AR_ActionSystemComponent.generated.h"

struct FAR_Attribute;
class UAR_AttributeSet;
class UAR_Action;

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
	
	virtual void InitializeComponent() override;
	
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
	
	UPROPERTY()
	TObjectPtr<UAR_AttributeSet> Attributes;	// The actual held attribute instance during runtime(cpp only,GC)
	
	TMap<FGameplayTag, FAR_Attribute*> CachedAttributes;

	// Config used to specify which 'UAR_AttributeSet' derived class to create
	UPROPERTY(EditAnywhere, Category = Attributes, NoClear)
	TSubclassOf<UAR_AttributeSet> AttributeSetClass;	
	
	UPROPERTY()
	TArray<TObjectPtr<UAR_Action>> Actions;
	
	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UAR_Action>> DefaultActions;

public:
	/*--------------- Attributes Relative ------------------*/
	/* Health */
	
	UPROPERTY(BlueprintAssignable)
	FOnHealthChanged OnHealthChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Setters")
	bool ApplyHealthChange(AActor* Instigator, float Delta);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	bool IsDead() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	bool IsFullHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | Getters")
	static UAR_ActionSystemComponent* GetASComp(AActor* FromActor);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes | ExecInterface")
	bool Kill(AActor* InstigatorActor);
	
	/*---------------------- Actions Relative ---------------------*/
	
	FGameplayTagContainer ActiveGameplayTags;
	
	void GrantAction(TSubclassOf<UAR_Action> NewActionClass);
	void StartAction(FGameplayTag InActionName);
	void StopAction(FGameplayTag InActionName);
	
	FAR_Attribute* GetAttribute(FGameplayTag InAttributeTag);
	
};
