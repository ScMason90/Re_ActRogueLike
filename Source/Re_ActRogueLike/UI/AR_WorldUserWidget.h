// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AR_WorldUserWidget.generated.h"

class USizeBox;
/**
 * 
 */
UCLASS()
class RE_ACTROGUELIKE_API UAR_WorldUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<AActor> AttachedActor;
	
	/* ---------------- Tweakable Render Params ----------------- */
	
	UPROPERTY(EditAnywhere, Category = "UI")
	float DistRenderThreshold = 2000.0f;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	float DistRenderScaleMultiplier = 1000.0f;
	
	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> ParentSizeBox;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
public:
	// Utility interface for set 'visibility' outside the class object
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetWidgetVisible(bool bVisible);
	
};
