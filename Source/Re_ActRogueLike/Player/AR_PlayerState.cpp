// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_PlayerState.h"

void AAR_PlayerState::AddCredits(int32 Delta)
{
	// Avoid user-error of adding a negative amount or zero
	if (!ensure(Delta > 0)) return;
	
	Credits += Delta;
	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool AAR_PlayerState::RemoveCredits(int32 Delta)
{
	// Avoid user-error of adding a negative amount or zero
	// ensure(Delta >= 0);
	if (Credits - Delta < 0 ) return false;	// Not enough credits available
	
	Credits -= Delta;
	OnCreditsChanged.Broadcast(this, Credits, -Delta);
	return true;
}

int32 AAR_PlayerState::GetCredits() const
{
	return Credits;
}