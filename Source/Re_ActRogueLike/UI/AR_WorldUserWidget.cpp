// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_WorldUserWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"

void UAR_WorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		UE_LOG(LogTemp, Warning, 
			TEXT("UAR_WorldUserWidget::NativeTick,AttachedActor no longer valid, removing Health Widget."));
		return;
	}
	
	FVector2D ScreenPosition;	// TODO: Add dot product check if 'AttachedActor' is not in player viewport?
	if (UGameplayStatics::ProjectWorldToScreen(
		GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition))
	{
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= Scale;
		
		if (ParentSizeBox) ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}
}
