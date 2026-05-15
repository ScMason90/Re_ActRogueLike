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
	
	FVector CameraLocation;
	FRotator CameraRotation;
	GetOwningPlayer()->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector ToActor = AttachedActor->GetActorLocation() - CameraLocation;
	ToActor.Normalize();

	if (const float Dot = FVector::DotProduct(CameraRotation.Vector(), ToActor); Dot < 0.f)
	{
		SetRenderOpacity(0.0f);
		return;
	}
	SetRenderOpacity(1.0f);

	const float Distance = FVector::Dist(CameraLocation, AttachedActor->GetActorLocation());
	if (Distance > 2000.f)
	{
		SetRenderOpacity(0.0f);
		return;
	}

	// Distance scaling: The farther away, the smaller
	const float ScaleFactor = FMath::Clamp(1000.f / Distance, 0.5f, 1.0f);
	if (ParentSizeBox) ParentSizeBox->SetRenderScale(FVector2D(ScaleFactor, ScaleFactor));
}
