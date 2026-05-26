// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"

#include "AR_Action.h"
#include "AR_AttributeSet.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Re_ActRogueLike/Core/AR_GameModeBase.h"
#include "../SharedGameplayTags.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"


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

void UAR_ActionSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Attributes->InitializeAttributes();
	
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
		break;
	case Modifier:
		FoundAttribute->Modifier += Delta;
		break;
	case OverrideBase:
		FoundAttribute->Base = Delta;
		break;
	default:
		check(false);
	}
	
	Attributes->PostAttributeChanged();
	
	// C++ Native Delegates
	if (FOnAttributeChanged* Event = AttributeListeners.Find(AttributeTag))
	{
		Event->Broadcast(AttributeTag, FoundAttribute->GetValue(), OldValue);
	}
	
	// Blueprint Delegates
	if (TArray<FOnAttributeDynamicChanged>* Events = AttributeDynamicListeners.Find(AttributeTag))
	{
		for (int i = Events->Num() - 1; i >= 0; --i)
		{
			FOnAttributeDynamicChanged& Event = (*Events)[i];
			bool bIsBound = Event.ExecuteIfBound(AttributeTag, FoundAttribute->GetValue(), OldValue);
			if (!bIsBound)
			{
				Events->RemoveAt(i);
				UE_LOG(LogTemp, Log, TEXT("UAR_ActionSystemComponent::ApplyAttributeChanged,"
							  "Clean up expired dynamic(BP) attribute delegate for %s"), *GetNameSafe(GetOwner()));
			}
		}
	}
	
	UE_LOGFMT(LogTemp, Log, "UAR_ActionSystemComponent::ApplyAttributeChanged, Attribute : {0}, New : {1}, Old : {2}",
		AttributeTag.ToString(), FoundAttribute->GetValue(), OldValue);
}

FAR_Attribute* UAR_ActionSystemComponent::GetAttribute(FGameplayTag InAttributeTag) const
{
	if (FAR_Attribute* const* FoundAttribute = CachedAttributes.Find(InAttributeTag)) return *FoundAttribute;
	
	return nullptr;
}

float UAR_ActionSystemComponent::GetAttributeValue(FGameplayTag InAttributeTag) const
{
	FAR_Attribute* FoundAttribute = GetAttribute(InAttributeTag);
	check(FoundAttribute);
	return FoundAttribute->GetValue();
}

FOnAttributeChanged& UAR_ActionSystemComponent::GetAttributeListener(FGameplayTag AttributeTag)
{
	return AttributeListeners.FindOrAdd(AttributeTag);
}

void UAR_ActionSystemComponent::AddDynamicAttributeListener(FOnAttributeDynamicChanged Event, FGameplayTag AttributeTag)
{
	TArray<FOnAttributeDynamicChanged>& Events = AttributeDynamicListeners.FindOrAdd(AttributeTag);
	Events.Add(Event);
}

void UAR_ActionSystemComponent::RemoveDynamicAttributeListener(FOnAttributeDynamicChanged Event)
{
	for (TPair<FGameplayTag, TArray<FOnAttributeDynamicChanged>>& Listener : AttributeDynamicListeners)
	{
		if (Listener.Value.RemoveSingle(Event) > 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("UAR_ActionSystemComponent::RemoveDynamicAttributeListener,"
								 "successfully removed blueprint binding."));
			break;
		}
	}
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
	ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, 
		GetAttribute(SharedGameplayTags::Attribute_HealthMax)->GetValue(), Base);
	
	return UAR_GameplayStatics::IsDead(this);
}
