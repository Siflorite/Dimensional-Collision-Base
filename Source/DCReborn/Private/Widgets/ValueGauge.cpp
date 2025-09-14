// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"

// #include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameplayEffectTypes.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(BarColor);
}

void UValueGauge::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
	if (AbilitySystemComponent)
	{
		bool bFound, bFoundMax;
		CachedValue = AbilitySystemComponent->GetGameplayAttributeValue(Attribute, bFound);
		CachedMaxValue = AbilitySystemComponent->GetGameplayAttributeValue(MaxAttribute, bFoundMax);
		if (!bFound || !bFoundMax)
		{
			return;
		}
		SetValue(CachedValue, CachedMaxValue);
		// Subscribe to changes of the attributes
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &UValueGauge::ValueChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &UValueGauge::MaxValueChanged);
	}
}

// 更新进度条和文本块UI
void UValueGauge::SetValue(const float NewValue, const float NewMaxValue) const
{
	if (NewMaxValue == 0)
	{
		// 除数不能为0
		UE_LOG(LogTemp, Warning, TEXT("Value Gauge: %s, NewMaxValue cannot be 0."), *GetName());
		return;
	}
	
	const float NewPercentage = NewValue / NewMaxValue;
	ProgressBar->SetPercent(NewPercentage);

	// 文本显示数值不显示小数位
	const FNumberFormattingOptions FormatOps = FNumberFormattingOptions().SetMaximumFractionalDigits(0);
	ValueText->SetText(
		FText::Format(
			FTextFormat::FromString("{0}/{1}"),
			FText::AsNumber(NewValue, &FormatOps),
			FText::AsNumber(NewMaxValue, &FormatOps)
		)
	);
}

void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangedData)
{
	CachedValue = ChangedData.NewValue;
	SetValue(CachedValue, CachedMaxValue);
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangedData)
{
	CachedMaxValue = ChangedData.NewValue;
	SetValue(CachedValue, CachedMaxValue);
}
