// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadStatsGauge.generated.h"

/**
 * 
 */
UCLASS()
class UOverheadStatsGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;
	
	void ConfigureAbilitySystemComponent(class UAbilitySystemComponent* AbilitySystemComponent) const;
	// 外部调用，对血条进行缩放。
	void SetOverheadWidgetScale(const float Scale) const;

private:
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* HealthBar;
	
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* ManaBar;

	UPROPERTY(meta=(BindWidget))
	class USizeBox* WidthBox;

	UPROPERTY(meta=(BindWidget))
	class USizeBox* HealthBox;

	UPROPERTY(meta=(BindWidget))
	class USizeBox* ManaBox;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float DefaultWidth = 250.f;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float DefaultHealthHeight = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float DefaultManaHeight = 10.f;
};
