// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_AttributeSet.h"

UAR_HealthAttributeSet::UAR_HealthAttributeSet()
{
	Health = FAR_Attribute(100.0f);
	HealthMax = FAR_Attribute(Health.GetValue());
}

void UAR_HealthAttributeSet::PostAttributeChanged()
{
	Health.Base = FMath::Clamp(Health.Base, 0.0f, HealthMax.GetValue());
}
