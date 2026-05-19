// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AR_AttributeSet.generated.h"

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
UCLASS()
class RE_ACTROGUELIKE_API UAR_AttributeSet : public UObject
{
	GENERATED_BODY()
	
public:
	
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
    
	UPROPERTY(EditAnywhere, Category = Attributes)
	FAR_Attribute Health;     // Current health
    
	UPROPERTY(EditAnywhere, Category = Attributes)
	FAR_Attribute HealthMax;  // Maximum health
	
	virtual void PostAttributeChanged() override;
	
};
