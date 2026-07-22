// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_GameplayStatics.h"

// Necessary compilation header files
#include "Re_ActRogueLike/ActionSystem/AR_AttributeSet.h"

// Temporarily headers enables Intelligent Completion & Highlighting functions of JetbrainsRider IDE to operate, thereby enhancing development efficiency



static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("game.DamageMultiplier"), 1.0f, 
	TEXT("Global Damage Modifier for OnHealthAttrChanged of ASComponent."), ECVF_Cheat);


// You can replace all 'ASComp->GetAttribute()' to 'ASComp->GetAttributeValue()' for convenience.

bool UAR_GameplayStatics::IsFullHealth(UAR_ActionSystemComponent* ASComp)
{
	check(ASComp);
	
	FAR_Attribute* Health = ASComp->GetAttribute(SharedGameplayTags::Attribute_Health);
	FAR_Attribute* HealthMax = ASComp->GetAttribute(SharedGameplayTags::Attribute_HealthMax);
	
	return FMath::IsNearlyEqual(Health->GetValue(), HealthMax->GetValue());
}

bool UAR_GameplayStatics::IsDying(UAR_ActionSystemComponent* ASComp)
{
	check(ASComp);
	
	float HealthValue = ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Health);
	
	return FMath::IsNearlyZero(HealthValue) /*|| HealthValue <= KINDA_SMALL_NUMBER*/;
}

bool UAR_GameplayStatics::IsActorAlive(AActor* ActorToCheck)
{
	if (IsValid(ActorToCheck))
	{
		if (UAR_ActionSystemComponent* ASComp = ActorToCheck->FindComponentByClass<UAR_ActionSystemComponent>())
		{
			return ASComp->GetAttributeValue(SharedGameplayTags::Attribute_Health) > 0.0f;
		}
	}
	
	return false;
}

void UAR_GameplayStatics::UnbindOnMulticastAttrChangedDel(UAR_ActionSystemComponent* ASComp,
	const FGameplayTag& AttributeTag, FDelegateHandle InDelHandle)
{
	if (!ASComp || !InDelHandle.IsValid() || !SharedGameplayTags::IsAttributeTag(AttributeTag)) return;
	
	// if (TargetHealthChangedEvent.IsBoundToObject(InUserObject))...
	
	// Temp fatal assert
	check(ASComp);

	TWeakObjectPtr<UAR_ActionSystemComponent> WeakASComp = ASComp;
	
	FOnAttributeChanged& Event = WeakASComp->GetAttributeListener(AttributeTag);
	Event.Remove(InDelHandle);
	InDelHandle.Reset();
}

FDelegateHandle UAR_GameplayStatics::BindOnMulticastAttrChangedLambda(UAR_ActionSystemComponent* ASComp,
	const FGameplayTag& AttributeTag, TFunction<void(FGameplayTag, float, float)> Callback)
{
	if (!ASComp || !SharedGameplayTags::IsAttributeTag(AttributeTag)) return FDelegateHandle();
	
	// Temp fatal assert
	check(ASComp);
	
	TWeakObjectPtr<UAR_ActionSystemComponent> WeakASComp = ASComp;
	
	FOnAttributeChanged& Event = WeakASComp->GetAttributeListener(AttributeTag);
	return Event.AddLambda([Callback](FGameplayTag Tag, float NewV, float OldV)
	{
		Callback(Tag, NewV, OldV);
	});
}

float UAR_GameplayStatics::GetDmgModifier()
{
	return CVarDamageMultiplier.GetValueOnGameThread();
}

bool UAR_GameplayStatics::Kill(AActor* InstigatorActor, UAR_ActionSystemComponent* TargetASComp)
{
	TargetASComp->ApplyAttributeChanged(SharedGameplayTags::Attribute_Health, 
		-TargetASComp->GetAttributeValue(SharedGameplayTags::Attribute_HealthMax), Base);
	
	return IsDying(TargetASComp);
}
