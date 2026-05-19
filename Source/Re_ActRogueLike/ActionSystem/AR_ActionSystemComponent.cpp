// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"

#include "AR_Action.h"
#include "AR_AttributeSet.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Re_ActRogueLike/Core/AR_GameModeBase.h"
#include "../SharedGameplayTags.h"

static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("game.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for ASComponent."), ECVF_Cheat);

// Sets default values for this component's properties
UAR_ActionSystemComponent::UAR_ActionSystemComponent()
{
	/*bWantsInitializeComponent should be set in class object constructor not in virtual InitializeComponent() override 
	which should be its consequence call*/ 
	bWantsInitializeComponent = true;
	
	AttributeSetClass = UAR_AttributeSet::StaticClass();
}

void UAR_ActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	Attributes = NewObject<UAR_AttributeSet>(this, AttributeSetClass);
	
	for (TFieldIterator<FStructProperty> PropIt(Attributes.GetClass()); PropIt; ++PropIt)
	{
		FAR_Attribute* FoundAttribute = PropIt->ContainerPtrToValuePtr<FAR_Attribute>(Attributes);
		
		FName AttributeTagName = FName("Attribute." + PropIt->GetName());
		FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);
		
		CachedAttributes.Add(AttributeTag, FoundAttribute);
	}
	
	// UE_LOGFMT(LogTemp, Error, 
	// 	"UAR_ActionSystemComponent::InitializeComponent(), Is DefaultActions empty?{Answer}", DefaultActions.IsEmpty()?"Yes":"No");
	for (TSubclassOf<UAR_Action> ActionClass : DefaultActions)
	{
		if (ensure(ActionClass)) GrantAction(ActionClass);
	}
}

void UAR_ActionSystemComponent::ApplyAttributeChanged(FGameplayTag AttributeTag, float Delta,
	EAttributeModifyType ModifyType)
{
	FAR_Attribute* FoundAttribute = GetAttribute(AttributeTag);
	check(FoundAttribute);
	
	float OldValue = FoundAttribute->GetValue();

	switch (ModifyType)
	{
	case Base:
		FoundAttribute->Base += Delta;
	case Modifier:
		FoundAttribute->Modifier += Delta;
	case OverrideBase:
		FoundAttribute->Base = Delta;
	default:
		check(false);
	}
	
	Attributes->PostAttributeChanged();
	
	if (FOnAttributeChanged* Event = AttributeListeners.Find(AttributeTag))
	{
		Event->Broadcast(AttributeTag, FoundAttribute->GetValue(), OldValue);
	}
	
	UE_LOGFMT(LogTemp, Log, "Attribute : {0}, New : {1}, Old : {2}",
		AttributeTag.ToString(),
		FoundAttribute->GetValue(),
		OldValue);
}

FAR_Attribute* UAR_ActionSystemComponent::GetAttribute(FGameplayTag InAttributeTag)
{
	if (FAR_Attribute** FoundAttribute = CachedAttributes.Find(InAttributeTag)) return *FoundAttribute;
	
	return nullptr;
}

FOnAttributeChanged& UAR_ActionSystemComponent::GetAttributeListener(FGameplayTag AttributeTag)
{
	return AttributeListeners.FindOrAdd(AttributeTag);
}

void UAR_ActionSystemComponent::GrantAction(TSubclassOf<UAR_Action> NewActionClass)
{
	UAR_Action* NewAction = NewObject<UAR_Action>(this, NewActionClass);
	Actions.Add(NewAction);
}

void UAR_ActionSystemComponent::StartAction(FGameplayTag InActionName)
{
	// UE_LOGFMT(LogCore, Warning, 
	// 	"UAR_ActionSystemComponent::StartAction,Is Actions empty?{Answer}", Actions.IsEmpty()?"Yes":"No");
	
	for (UAR_Action* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			if (Action->CanStart()) Action->StartAction();
			return;
		}
	}
	
	UE_LOG(LogTemp, Warning, 
		TEXT("UAR_ActionSystemComponent::StartAction,No Action found with name %s"), *InActionName.ToString());
}

void UAR_ActionSystemComponent::StopAction(FGameplayTag InActionName)
{
	// UE_LOGFMT(LogCore, Warning, 
	// 	"UAR_ActionSystemComponent::StopAction,Is Actions empty?{Answer}", Actions.IsEmpty()?"Yes":"No");
	
	for (UAR_Action* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			Action->StopAction();
			return;
		}
	}
	
	UE_LOG(LogTemp, Warning, 
		TEXT("UAR_ActionSystemComponent::StopAction,No Action found with name %s"), *InActionName.ToString());
}

UAR_ActionSystemComponent* UAR_ActionSystemComponent::GetASComp(AActor* FromActor)
{
	if (FromActor) return Cast<UAR_ActionSystemComponent>(FromActor->GetComponentByClass(StaticClass()));
	return nullptr;
}

bool UAR_ActionSystemComponent::Kill(AActor* InstigatorActor)
{
	return true;//ApplyHealthChange(InstigatorActor, -100.0f/*-GetMaxHealth()*/);
}
