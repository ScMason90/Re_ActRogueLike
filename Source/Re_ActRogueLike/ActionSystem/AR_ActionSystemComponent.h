// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "AR_ActionSystemComponent.generated.h"

struct FAR_Attribute;
class UAR_AttributeSet;
class UAR_Action;



UENUM(BlueprintType)
enum EAttributeModifyType
{
	/* Correspond struct member variable in FAR_Attribute(Define in AR_Attribute.h) */Base,
	/* …FAR_Attribute(Define in AR_Attribute.h) */Modifier,
	/* Specify a new value to override 'Base'*/OverrideBase,
	/* ErrorType */Invalid
};



// Native C++ Delegates
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnAttributeChanged, 
	FGameplayTag/*AttributeTag*/, float/*NewAttributeValue*/, float/*OldAttributeValue*/)

// Blueprint Delegates
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnAttributeDynamicChanged, 
	FGameplayTag, AttributeTag, float, NewAttributeValue, float, OldAttributeValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayTagCountChanged, FGameplayTag, UpdatedTag, int32, NewCount);



/*
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), HideCategories = (Navigation, Cooking, Tags))
class RE_ACTROGUELIKE_API UAR_ActionSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UAR_ActionSystemComponent();
	
	virtual void InitializeComponent() override;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
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
	
	UPROPERTY(EditAnywhere, Instanced, NoClear, Category = ActionSystem)
	TObjectPtr<UAR_AttributeSet> Attributes;	// The actual held attribute instance during runtime(cpp only,GC)
	
	TMap<FGameplayTag, FAR_Attribute*> CachedAttributes;
	
	// C++ Native Listeners
	TMap<FGameplayTag, FOnAttributeChanged> AttributeListeners;
	
	// Blueprint Listeners
	TMap<FGameplayTag, TArray<FOnAttributeDynamicChanged>> AttributeDynamicListeners; 
	
	UPROPERTY()
	TArray<TObjectPtr<UAR_Action>> Actions;
	
	UPROPERTY(EditAnywhere, Category = ActionSystem)
	TArray<TSubclassOf<UAR_Action>> DefaultActions;
	
	FGameplayTagContainer ActiveGameplayTags;
	
	void CheckAgainstBlockedTags(const FGameplayTagContainer& NewTags);

public:
	
	/*--------------- Attributes & Actions Relative ------------------*/
	
	FAR_Attribute* GetAttribute(FGameplayTag InAttributeTag) const;
	
	UFUNCTION(BlueprintCallable)
	float GetAttributeValue(FGameplayTag InAttributeTag) const;
	
	FOnAttributeChanged& GetAttributeListener(FGameplayTag AttributeTag);
	
	UFUNCTION(BlueprintCallable, DisplayName = "Add Dynamic Attribute Listener", meta = (Keywords = "events,delegate"))
	void AddDynamicAttributeListener(FOnAttributeDynamicChanged Event, FGameplayTag AttributeTag);
	
	UFUNCTION(BlueprintCallable, DisplayName = "Remove Dynamic Attribute Listener", meta = (Keywords = "events,delegate"))
	void RemoveDynamicAttributeListener(FOnAttributeDynamicChanged Event);
	
	/**
	 * @param ModifyType Check Source Enum for details.Whether it is C++ or a blueprint, this parameter must be specified; otherwise, an error will be triggered from the check(false) enumeration result within the function.
	 */
	UFUNCTION(BlueprintCallable)
	void ApplyAttributeChanged(FGameplayTag AttributeTag, float Delta, EAttributeModifyType ModifyType);
	
	/**
	 * @param NewActionClass Directly apply effect ('NewAction->StartAction()') if it's derived from 'UAR_Effect'
	 */
	UFUNCTION(BlueprintCallable)
	void GrantAction(TSubclassOf<UAR_Action> NewActionClass);
	
	void RemoveAction(UAR_Action* ActionToRemove);
	
	void StartAction(FGameplayTag InActionName);
	void StopAction(FGameplayTag InActionName);
	
	// we do you say build TMap<ActionName, TObjectPtr<UAR_Action>> for better performance?
	// const TArray<TObjectPtr<UAR_Action>>& GetActions() const {return Actions;}  
	
	void SetDefaultAttributeSet(TSubclassOf<UAR_AttributeSet> AttributeSetClass);
	
	void AppendActiveTags(FGameplayTagContainer NewTags);
	
	void RemoveActiveTags(FGameplayTagContainer TagsToRemove);
	
	UPROPERTY(BlueprintAssignable)
	FOnGameplayTagCountChanged OnGameplayTagCountUpdated;
	
	const FGameplayTagContainer& GetActiveTags() const {return ActiveGameplayTags;}
	
	/** Manually stop all potential processing/activating 'Actions' or 'Effects'
	 * @note Mainly called when owner is dying -> lifespan expired / destroying / being GC.
	 * @warning Create another (world level)'Manager' class of persistent Action/Effect that are cross-character&system or retained after death (Usually destroy/disable outside owner's life cycle) */
	void EndActionsAndEffects();

	/** Only for debug/development (Log Output). Not sure of embed in ActionSystem.
	 * @param InAction UAR_Action and its derived object - Could be UAR_Effect and its derived object as that UAR_Action derived UAR_Effect.  
	 * @return Action as "Action_...". StatusEffect as "StatusEffect_..." */
	FString GetActionNameOrGrantTags(UAR_Action* InAction);
	
};
