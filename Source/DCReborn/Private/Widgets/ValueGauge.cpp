// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ValueGauge.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UValueGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	ProgressBar->SetFillColorAndOpacity(BarColor);
}

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
