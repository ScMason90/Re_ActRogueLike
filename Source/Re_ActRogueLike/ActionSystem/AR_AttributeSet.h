// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AR_AttributeSet.generated.h"


class UAR_ActionSystemComponent;


/**
 * Attribute unit structure.
 * Holds a base value and a modifier, used to calculate the final attribute value.
 */
USTRUCT()
struct FAR_Attribute
{
	GENERATED_BODY()
    
	FAR_Attribute() {}
    
	FAR_Attribute(float InBase) 
		: Base(InBase) {}

	UPROPERTY(EditAnywhere)
	float Base = 0.0f;       // Base value
    
	UPROPERTY(Transient)
	float Modifier = 0.0f;   // Temporary modifier - Extern Accessible
    
	float GetValue() {return Base + Modifier;}
};


/**
 * Base class for attribute sets.
 * Provides a common parent for all specific attribute collections.
 */
UCLASS(EditInlineNew)
class RE_ACTROGUELIKE_API UAR_AttributeSet : public UObject
{
	GENERATED_BODY()
	
public:
	
	/** Since we load 'UAR_AttributeSet' and 'UAR_Action' or their derived classes only in 'UAR_ActionSystemComponent'
	 * , No need of any error check and validation.Keep that in mind we always restore database and
	 * execute relative application in 'ASComp' */
	UAR_ActionSystemComponent* GetOwningASComponent() const;
	
	virtual void InitializeAttributes() {};
	
	virtual void PostAttributeChanged() {};
	
};


/**
 * Health attribute set.
 * Defines health-related attributes such as current health and maximum health.
 */
UCLASS()
class UAR_HealthAttributeSet : public UAR_AttributeSet
{
	GENERATED_BODY()

public:
	
	UAR_HealthAttributeSet();
    
	UPROPERTY(EditAnywhere, Category = Health)
	FAR_Attribute Health;     // Current health
    
	UPROPERTY(EditAnywhere, Category = Health)
	FAR_Attribute HealthMax;  // Maximum health
	
	virtual void PostAttributeChanged() override;
	
};

// A 'UAR_DummyAttributeSet' only for test?

/**
 *
 */
UCLASS()
class UAR_PawnAttributeSet : public UAR_HealthAttributeSet
{
	GENERATED_BODY()

public:
	
	UAR_PawnAttributeSet();
	
	/**
	 * Walk speed directly linked with Character Movement Component
	 */
	UPROPERTY(EditAnywhere, Category = MoveSpeed)
	FAR_Attribute MoveSpeed;
	
	UPROPERTY(EditAnywhere, Category = MoveSpeed)
	FAR_Attribute MoveSpeedMultiplier;
	
	virtual void InitializeAttributes() override;
	
	virtual void PostAttributeChanged() override;
	
	void ApplyMoveSpeed();
	
};

/**
 *
 */
UCLASS()
class UAR_PlayerAttributeSet : public UAR_PawnAttributeSet
{
	GENERATED_BODY()

public:
	
	UAR_PlayerAttributeSet();
	
	UPROPERTY(EditAnywhere, Category = Resources)
	FAR_Attribute Rage;
	
};

/**
 *
 */
UCLASS()
class UAR_EnemyAttributeSet : public UAR_PawnAttributeSet
{
	GENERATED_BODY()

public:
	
	UAR_EnemyAttributeSet();
	
};
