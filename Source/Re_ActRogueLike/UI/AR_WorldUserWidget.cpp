// Fill out your copyright notice in the Description page of Project Settings.


#include "AR_WorldUserWidget.h"

#include "VectorTypes.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SizeBox.h"
#include "Kismet/GameplayStatics.h"
#include "Re_ActRogueLike/Re_ActRogueLike.h"

void UAR_WorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();
		UE_LOG(LogGame, Warning, 
			TEXT("UAR_WorldUserWidget::NativeTick,AttachedActor no longer valid, removing Health Widget."));
		return;
	}
	
	FVector2D ScreenPosition;
	if (UGameplayStatics::ProjectWorldToScreen(
		GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition))
	{
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);
		ScreenPosition /= Scale;
		
		SetRenderOpacity(1.0f);
		if (ParentSizeBox) ParentSizeBox->SetRenderTranslation(ScreenPosition);
	}
	else
	{
		SetRenderOpacity(0.0f);
		return;
	}
	
	// Dot product: 'Hide' if the widget is outside the viewport
	FVector CameraLocation;
	FRotator CameraRotation;
	GetOwningPlayer()->GetPlayerViewPoint(CameraLocation, CameraRotation);
	
	FVector ToAttachedActor = AttachedActor->GetActorLocation() - CameraLocation;
	ToAttachedActor.Normalize();
	
	if (const float Dot = FVector::DotProduct(ToAttachedActor, CameraRotation.Vector()); Dot <= 0.0f)
	{
		SetRenderOpacity(0.0f);
		return;
	}

	// Distance Threshold: 'Hide' if overstepped
	const float Distance = FVector::Dist(AttachedActor->GetActorLocation(), CameraLocation);
	if (Distance > DistRenderThreshold)
	{
		SetRenderOpacity(0.0f);
		return;
	}
	
	// Distance scaling: The farther away, the smaller
	const float ScaleFactor = FMath::Clamp(DistRenderScaleMultiplier / Distance, 0.5f, 1.0f);
	if (ParentSizeBox) ParentSizeBox->SetRenderScale(FVector2D(ScaleFactor, ScaleFactor));
	
	SetRenderOpacity(1.0f);
}

void UAR_WorldUserWidget::SetWidgetVisible(bool bVisible) {SetRenderOpacity(bVisible ? 1.0f : 0.0f);}
