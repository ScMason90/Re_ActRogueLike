// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"

// Necessary compilation header files
#include "AR_Action.h"
#include "AR_AttributeSet.h"
#include "AR_Effect.h"
#include "Re_ActRogueLike/GameModes/AR_GameModeBase.h"
#include "../SharedGameplayTags.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



// Sets default values for this component's properties
UAR_ActionSystemComponent::UAR_ActionSystemComponent()
{
	/*bWantsInitializeComponent should be set in class object constructor not in virtual InitializeComponent() override 
	which should be its consequence call*/ 
	bWantsInitializeComponent = true;
	
}

void UAR_ActionSystemComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	// Fallback for BP and CPP having not yet defined a default
	if (Attributes == nullptr)
	{
		Attributes = NewObject<UAR_AttributeSet>(this, UAR_AttributeSet::StaticClass());
		UE_LOG(LogGame, Warning, TEXT("%s No default 'AttributeSet' defined. Set using 'SetDefaultAttributeSet()'"
			"during Actor Construction or assign in Blueprint 'ActionComponent' for %s."), *AR_DEBUG_LOC(), *GetNameSafe(GetOwner()));
	}
	
	for (TFieldIterator<FStructProperty> PropIt(Attributes.GetClass()); PropIt; ++PropIt)
	{
		FAR_Attribute* FoundAttribute = PropIt->ContainerPtrToValuePtr<FAR_Attribute>(Attributes);
		
		FName AttributeTagName = FName("Attribute." + PropIt->GetName());
		FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);
		
		CachedAttributes.Add(AttributeTag, FoundAttribute);
	}
	
}

void UAR_ActionSystemComponent::SetDefaultAttributeSet(TSubclassOf<UAR_AttributeSet> AttributeSetClass)
{
	check(!HasBeenInitialized());
	
	// Only available during constructors of UObject
	FObjectInitializer& ObjectInitializer = FObjectInitializer::Get();
	Attributes = Cast<UAR_AttributeSet>(ObjectInitializer.CreateDefaultSubobject(
		this, TEXT("Attributes"), AttributeSetClass, AttributeSetClass));
}

void UAR_ActionSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	Attributes->InitializeAttributes();
	
	// UE_LOGFMT(LogGame, Warning, "{LogLoc} DefaultActions.IsEmpty()? {Answer}", *AR_DEBUG_LOC(), DefaultActions.IsEmpty()?"Yes":"No");
	for (TSubclassOf<UAR_Action> ActionClass : DefaultActions)
	{
		if (ensure(ActionClass)) GrantAction(ActionClass);
	}
}

void UAR_ActionSystemComponent::ApplyAttributeChanged(FGameplayTag AttributeTag, float Delta, EAttributeModifyType ModifyType)
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
				Events->RemoveAtSwap(i, EAllowShrinking::No);
				UE_LOG(LogGame, Log, TEXT("%s Clean up expired dynamic(BP) attribute delegate for %s"), 
					*AR_DEBUG_LOC(), *GetNameSafe(GetOwner()));
			}
		}
	}
	
	UE_LOGFMT(LogGame, Log, "{4} Owner : {0}; Attribute : {1}, Old : {3}, New : {2}", GetNameSafe(GetOwner()), 
		AttributeTag.ToString(), OldValue, FoundAttribute->GetValue(), *AR_DEBUG_LOC());
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
			UE_LOG(LogGame, Warning, TEXT("%s successfully removed blueprint binding."), *AR_DEBUG_LOC());
			break;
		}
	}
}

void UAR_ActionSystemComponent::GrantAction(TSubclassOf<UAR_Action> NewActionClass)
{
	const bool bIsEffectClass = NewActionClass->IsChildOf(UAR_Effect::StaticClass());
	if (bIsEffectClass)
	{
		// Find existing debuff by class, you could have different 'StackingBehavior' e.g. allowing one debuff class PER instigator
		// Note: Buffs and Actions may desire their own individual arrays when expanding on the Action System.
		for (UAR_Action* Action : Actions)
		{
			if (UAR_Effect* Effect = Cast<UAR_Effect>(Action))
			{
				if (Effect->GetClass() == NewActionClass)
				{
					Effect->IncrementStackSize();
					return;
				}	
			}
		}
	}
	
	UAR_Action* NewAction = NewObject<UAR_Action>(this, NewActionClass);
	Actions.Add(NewAction);
	
	if (bIsEffectClass)
	{
		// Sanity check that buffs are allowed to run. We do not handle this case yet.
		ensureMsgf(NewAction->CanStart(), TEXT("%s an Effect can not start CanStart() returns FALSE. Case not handled."), *AR_DEBUG_LOC());
		
		UE_LOG(LogGame, Log, TEXT("%s, Owner:%s NewActionNameOrGrantTags : %s."), 
			*AR_DEBUG_LOC(), *GetNameSafe(GetOwner()), *GetActionNameOrGrantTags(NewAction));	
		
		NewAction->StartAction();
	}
}

void UAR_ActionSystemComponent::RemoveAction(UAR_Action* ActionToRemove)
{
	int32 RemoveCount = Actions.RemoveSingle(ActionToRemove);
	ensure(RemoveCount == 1);
	
	UE_LOG(LogGame, Verbose, TEXT("%s Removed Action %s from %s"), *AR_DEBUG_LOC(), 
		*GetActionNameOrGrantTags(ActionToRemove), *GetNameSafe(GetOwner()));
	
	ActionToRemove->MarkAsGarbage();
}

void UAR_ActionSystemComponent::AppendActiveTags(FGameplayTagContainer NewTags)
{
	ActiveGameplayTags.AppendTags(NewTags);
	
	CheckAgainstBlockedTags(NewTags);
	
	for (FGameplayTag Tag : NewTags)
	{
		OnGameplayTagCountUpdated.Broadcast(Tag, 1);
	}
}

void UAR_ActionSystemComponent::RemoveActiveTags(FGameplayTagContainer TagsToRemove)
{
	int32 PrevCount = ActiveGameplayTags.Num();
	
	ActiveGameplayTags.RemoveTags(TagsToRemove);
	
	ensure(PrevCount - ActiveGameplayTags.Num() == TagsToRemove.Num());
	
	for (FGameplayTag Tag : TagsToRemove)
	{
		OnGameplayTagCountUpdated.Broadcast(Tag, 0);
	}
}

void UAR_ActionSystemComponent::CheckAgainstBlockedTags(const FGameplayTagContainer& NewTags)
{
	for (UAR_Action* Action : Actions)
	{
		if (Action->IsRunning() && NewTags.HasAny(Action->GetBlockedTags()))
		{
			Action->StopAction();
			
			UE_LOGFMT(LogGame, Log, "{LogLoc}, Stopped {ActionName} due to any matching tag {BlockedTags} for {Owner}",
				("LogLoc", *AR_DEBUG_LOC()), ("ActionName", GetActionNameOrGrantTags(Action)), 
				("BlockedTags", NewTags.ToString()), ("Owner", GetNameSafe(GetOwner())));
		}
	}
}

void UAR_ActionSystemComponent::StartAction(FGameplayTag InActionName)
{
	for (UAR_Action* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			if (Action->CanStart()) Action->StartAction();
			return;
		}
	}
	
	UE_LOG(LogGame, Warning, TEXT("%s No Action found with name %s"), *AR_DEBUG_LOC(), *InActionName.ToString());
}

void UAR_ActionSystemComponent::StopAction(FGameplayTag InActionName)
{
	for (UAR_Action* Action : Actions)
	{
		if (Action->GetActionName() == InActionName)
		{
			if (Action->IsRunning()) Action->StopAction(); 
			return;
		}
	}
	
	UE_LOG(LogGame, Warning, TEXT("%s No Action found with name %s"), *AR_DEBUG_LOC(), *InActionName.ToString());
}

void UAR_ActionSystemComponent::EndActionsAndEffects()
{
	FString OwnerName = GetOwner()->GetActorNameOrLabel();
	
	// Actions
	for (int32 i = Actions.Num() - 1; i >= 0; --i)
	{
		UAR_Action* StoredAction = Actions[i];
		
		StopAction(StoredAction->GetActionName());
		
		UE_LOG(LogGame, Log, TEXT("%s Owner:%s StopAction(StoredAction--%s)"), 
			*AR_DEBUG_LOC(), *OwnerName, *GetActionNameOrGrantTags(StoredAction));
	}
	
	// Status_Effect/'ActionEffects(Buff/Debuff)'
	RemoveActiveTags(GetActiveTags());
	
	// No need delete 'Actions' that contains all actions or 'ActiveGameplayTags' that records all effects(also action's GrantTags)
	// As ASComp will be GC together with its OwnerActor (Prerequisite is that you only call this function when OwnerActor is dying) 
}

FString UAR_ActionSystemComponent::GetActionNameOrGrantTags(UAR_Action* InAction)
{
#if !UE_BUILD_SHIPPING
	check(InAction);
	if (InAction->GetActionName().IsValid())
	{
		return InAction->GetActionName().ToString();	// InAction is a UAR_Action or its derived.
	}
	return InAction->GetGrantTagsAsString();	// InAction is a UAR_Effect or its derived. Make sure you at least specify GrantTags.
#else
	return TEXT("");	// Nothing when Shipping as playable game
#endif
}
