// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Re_ActRogueLike/SharedGameplayTags.h"
#include "Re_ActRogueLike/ActionSystem/AR_ActionSystemComponent.h"
// #include "Re_ActRogueLike/Core/AR_GameplayStatics.tpp"	// How to better settle Template Definition tpp file in UE5 Cpp?
#include "AR_GameplayStatics.generated.h"

// Convenient marco for bind/unbind a FOnAttributeChanged multicast delegate - Please debug in calling function rather marco
#define UAR_GameplayStatics_BIND_ATTR_MULTICAST(Obj, Func, ASComp, Tag) \
	UAR_GameplayStatics::BindOnMulticastAttrChangedDel(Obj, Func, ASComp, Tag);
#define UAR_GameplayStatics_UNBIND_ATTR_MULTICAST(ASComp, Tag, DelHandle) \
	UAR_GameplayStatics::UnbindOnMulticastAttrChangedDel(ASComp, Tag, DelHandle);

struct FGameplayTag;
class UAR_ActionSystemComponent;

/** 
 * Even so, the specific logic of all the utility functions related to ActionSystem still depends on your game design
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_GameplayStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	/**
	 * @param ASComp ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 */
	UFUNCTION(BlueprintCallable, Category = Action_System)
	static bool IsFullHealth(UAR_ActionSystemComponent* ASComp);
	
	/**	Return FMath::IsNearlyZero(HealthValue)...Uncomment '|| HealthValue < 0.0f' if you want. 
	 * @param ASComp ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 */
	UFUNCTION(BlueprintCallable, Category = Action_System)
	static bool IsDying(UAR_ActionSystemComponent* ASComp);
	
	// If game design allow health less than Zero to be valid, then you might need another functionality as checkpoint
	
	/**
	 * @return false if ActorToCheck didn't have a 'ASComp' or its 'ASComp's HealthValue > 0.0f.Also, false if it's invalid actor.
	 * @param ActorToCheck A C++ pointer.Assign to the specific actor that you want to check if it's alive. 
	 */
	UFUNCTION(BlueprintCallable, Category = Action_System)
	static bool IsActorAlive(AActor* ActorToCheck);
	
	/** Bind GameplayTag specified MULTICAST_DELEGATE subscription on given ActionSystemComponent.
	 * @note Param 'GameplayTag' could be any Tag in our ActionSystem e.g. 'Attribute', 'Action', 'StatusEffect' each corresponding a MULTICAST_DELEGATE
	 * @param InUserObj User object to bind to
	 * @param Func Class method function address
	 * @param ASComp Must be a valid ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 * @param AttributeTag Desired Attribute (Depends on calling UseClass's AttributeSet Of 'ASComp')
	 * @return A FDelegateHandle of multicast binding result
	 */
	template<typename UserClass>
	static FDelegateHandle BindOnMulticastAttrChangedDel(UserClass* InUserObj, void (UserClass::*Func)(FGameplayTag, float, float), 
		UAR_ActionSystemComponent* ASComp, const FGameplayTag& AttributeTag);
	
	/** Unbind GameplayTag specified MULTICAST_DELEGATE subscription on given ActionSystemComponent. 
	 * @note Didn't check if 'InDelHandle' was bind to ASComp's owner 'UserObject' now
	 * @param ASComp Must be a valid ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 * @param AttributeTag Desired Attribute (Depends on calling UseClass's AttributeSet Of 'ASComp')
	 * @param InDelHandle Delegate Handle that bind a multicast FOnAttributeChanged event on 'ASComp' within calling UserClass 
	 */
	static void UnbindOnMulticastAttrChangedDel(UAR_ActionSystemComponent* ASComp, const FGameplayTag& AttributeTag, 
		FDelegateHandle InDelHandle);

	/** Flexibly capture calling context, but manual unbinding is required
	 * @warning Must manually unbind result Delegate Handle.(No auto-unbind when UserObj life span expired) 
	 * @param ASComp Must be a valid ActionSystemComponent derived from UAR_ActionSystemComponent class.
	 * @param AttributeTag Desired Attribute (Depends on calling UseClass's AttributeSet Of 'ASComp')
	 * @param Callback Calling function to bind with
	 * @return A FDelegateHandle of multicast binding result
	 */
	static FDelegateHandle BindOnMulticastAttrChangedLambda(UAR_ActionSystemComponent* ASComp, const FGameplayTag& AttributeTag, 
		TFunction<void(FGameplayTag, float, float)> Callback);
	
	// Getter of Global Damage Multiplier 'CVarDamageMultiplier' define in AR_GameplayStatics.cpp
	static float GetDmgModifier();		// Expose to BP?
	
	UFUNCTION(BlueprintCallable, Category = "ExecInterface")
	static bool Kill(AActor* InstigatorActor, UAR_ActionSystemComponent* TargetASComp);
	
};

template <typename UserClass>
FDelegateHandle UAR_GameplayStatics::BindOnMulticastAttrChangedDel(UserClass* InUserObj,
	void(UserClass::* Func)(FGameplayTag, float, float), UAR_ActionSystemComponent* ASComp,
	const FGameplayTag& AttributeTag)
{
	if (!ASComp || !InUserObj || !Func || !SharedGameplayTags::IsAttributeTag(AttributeTag)) return FDelegateHandle();
	// TODO: Add independent debug output for each 'true' state returned ?
	
	// Temp fatal assert
	check(ASComp);
	check(InUserObj);
	check(Func);
	
	TWeakObjectPtr<UserClass> WeakThis = InUserObj;
	TWeakObjectPtr<UAR_ActionSystemComponent> WeakASComp = ASComp;
	
	FOnAttributeChanged& Event = WeakASComp->GetAttributeListener(AttributeTag);
	return Event.AddUObject(WeakThis.Get(), Func);
}
