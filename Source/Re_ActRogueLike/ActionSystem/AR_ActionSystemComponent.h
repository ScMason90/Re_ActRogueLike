// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AR_ActionSystemComponent.generated.h"

struct FAR_Attribute;
class UAR_AttributeSet;
class UAR_Action;

UENUM()
enum EAttributeModifyType
{
	/* Correspond struct member variable in FAR_Attribute(Define in AR_Attribute.h) */Base,
	/* …FAR_Attribute(Define in AR_Attribute.h) */Modifier,
	/* Specify a new value to override 'Base'*/OverrideBase,
	/* ErrorType */Invalid
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, 
	FGameplayTag/*AttributeTag*/, float/*NewAttributeValue*/, float/*OldAttributeValue*/)

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
	// VisibleAnywhere - 'read-only' in editor and level. (Use for Components).Still useful to view in-editor and enforce a convention.
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
	
	TMap<FGameplayTag, FOnAttributeChanged> AttributeListeners;
	
	UPROPERTY()
	TArray<TObjectPtr<UAR_Action>> Actions;
	
	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TSubclassOf<UAR_Action>> DefaultActions;

public:
	/*--------------- Attributes & Actions Relative ------------------*/
	
	FAR_Attribute* GetAttribute(FGameplayTag InAttributeTag);
	
	FOnAttributeChanged& GetAttributeListener(FGameplayTag AttributeTag);
	
	void ApplyAttributeChanged(FGameplayTag AttributeTag, float Delta, EAttributeModifyType ModifyType);
	
	UFUNCTION(BlueprintCallable, Category = "Utilities")
	static UAR_ActionSystemComponent* GetASComp(AActor* FromActor);
	
	FGameplayTagContainer ActiveGameplayTags;
	
	void GrantAction(TSubclassOf<UAR_Action> NewActionClass);
	void StartAction(FGameplayTag InActionName);
	void StopAction(FGameplayTag InActionName);
	
	/*------------- Legacy unfixed ----------------*/
	
	UFUNCTION(BlueprintCallable, Category = "ExecInterface")
	bool Kill(AActor* InstigatorActor);
	
};
