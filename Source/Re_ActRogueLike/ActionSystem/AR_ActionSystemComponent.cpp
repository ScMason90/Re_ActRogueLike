// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_ActionSystemComponent.h"

#include "AR_Action.h"
#include "AR_AttributeSet.h"
#include "AR_Effect.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Logging/StructuredLog.h"
#include "Re_ActRogueLike/Core/AR_GameModeBase.h"
#include "../SharedGameplayTags.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"
#include "Re_ActRogueLike/Core/AR_GameplayStatics.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("game.DamageMultiplier"), 1.0f, TEXT("Global Damage Modifier for ASComponent."), ECVF_Cheat);


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
		UE_LOG(LogGame, Warning, TEXT("UAR_ActionSystemComponent::InitializeComponent(),No default 'AttributeSet' defined. Set using 'SetDefaultAttributeSet()'"
								"during Actor Construction or assign in Blueprint 'ActionComponent' for %s."), *GetNameSafe(GetOwner()));
	}
	
	for (TFieldIterator<FStructProperty> PropIt(Attributes.GetClass()); PropIt; ++PropIt)
	{
		FAR_Attribute* FoundAttribute = PropIt->ContainerPtrToValuePtr<FAR_Attribute>(Attributes);
		
		FName AttributeTagName = FName("Attribute." + PropIt->GetName());
		FGameplayTag AttributeTag = FGameplayTag::RequestGameplayTag(AttributeTagName);
		
		CachedAttributes.Add(AttributeTag, FoundAttribute);
	}
	
	// UE_LOGFMT(LogGame, Error, 
	// 	"UAR_ActionSystemComponent::InitializeComponent(), Is DefaultActions empty?{Answer}", DefaultActions.IsEmpty()?"Yes":"No");
	for (TSubclassOf<UAR_Action> ActionClass : DefaultActions)
	{
		if (ensure(ActionClass)) GrantAction(ActionClass);
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
				Events->RemoveAtSwap(i, EAllowShrinking::No);
				UE_LOG(LogGame, Log, TEXT("UAR_ActionSystemComponent::ApplyAttributeChanged,"
							  "Clean up expired dynamic(BP) attribute delegate for %s"), *GetNameSafe(GetOwner()));
			}
		}
	}
	
	UE_LOGFMT(LogGame, Log, "UAR_ActionSystemComponent::ApplyAttributeChanged; Owner : {0}; Attribute : {1}, Old : {3}, New : {2}",
		GetNameSafe(GetOwner()), AttributeTag.ToString(), OldValue, FoundAttribute->GetValue());
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
			UE_LOG(LogGame, Warning, TEXT("UAR_ActionSystemComponent::RemoveDynamicAttributeListener,"
								 "successfully removed blueprint binding."));
			break;
		}
	}
}

void UAR_ActionSystemComponent::GrantAction(TSubclassOf<UAR_Action> NewActionClass)
{
	FString NewEffectName = "Undefined";
	
	const bool bIsEffectClass = NewActionClass->IsChildOf(UAR_Effect::StaticClass());
	if (bIsEffectClass)
	{
		// Find existing debuff by class, you could have different 'StackingBehavior' e.g. allowing one debuff class PER instigator
		// Note: Buffs and Actions may desire their own individual arrays when expanding on the Action System.
		for (UAR_Action* Action : Actions)
		{
			if (UAR_Effect* Effect = Cast<UAR_Effect>(Action))
			{
				NewEffectName = Effect->GetActionName().ToString();
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
		if (ensureMsgf(NewAction->CanStart(), TEXT("UAR_ActionSystemComponent::GrantAction, "
											 "an Effect can not start CanStart() returns FALSE. Case not handled.")))
		{
			UE_LOG(LogGame, Log, TEXT("UAR_ActionSystemComponent::GrantAction,"
							 "This Effect has name(ActionName in UAR_Action.h): %s."), *NewEffectName);	
		}	
		
		NewAction->StartAction();
	}
}

void UAR_ActionSystemComponent::RemoveAction(UAR_Action* ActionToRemove)
{
	int32 RemoveCount = Actions.RemoveSingle(ActionToRemove);
	ensure(RemoveCount == 1);
	
	FString ActN = ActionToRemove->GetActionName().IsValid() ? ActionToRemove->GetActionName().ToString() : "Undefined Action Name";
	UE_LOG(LogGame, Verbose, TEXT("UAR_ActionSystemComponent::RemoveAction(UAR_Action* ActionToRemove), Remove Action %s from %s"), 
		*ActN, *GetNameSafe(GetOwner()));
	
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
			
			UE_LOGFMT(LogGame, Log, "UAR_ActionSystemComponent::CheckAgainstBlockedTags, "
						   "Stopped {ActionName} due to any matching tag {BlockedTags} for {Owner}",
						   ("ActionName", Action->GetActionName().ToString()),
						   ("BlockedTags", NewTags.ToString()),
						   ("Owner", GetNameSafe(GetOwner())));
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
	
	UE_LOG(LogGame, Warning, 
		TEXT("UAR_ActionSystemComponent::StartAction,No Action found with name %s"), *InActionName.ToString());
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
	
	UE_LOG(LogGame, Warning, 
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
		-GetAttribute(SharedGameplayTags::Attribute_HealthMax)->GetValue(), Base);
	
	return UAR_GameplayStatics::IsDying(this);
}
