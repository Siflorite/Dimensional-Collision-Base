// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValueGauge.generated.h"

/**
 * 
 */
UCLASS()
class UValueGauge : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

	void SetAndBoundToGameplayAttribute(class UAbilitySystemComponent* AbilitySystemComponent, const struct FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute);
	void SetValue(const float NewValue, const float NewMaxValue) const;

private:
	UPROPERTY(EditAnywhere, Category= "Visual")
	FLinearColor BarColor;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UProgressBar* ProgressBar;
	
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	class UTextBlock* ValueText;

	float CachedValue;
	float CachedMaxValue;

	void ValueChanged(const struct FOnAttributeChangeData& ChangedData);
	void MaxValueChanged(const struct FOnAttributeChangeData& ChangedData);
};
