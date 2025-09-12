// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverheadStatsGauge.h"

#include "AbilitySystemComponent.h"
#include "Components/SizeBox.h"
#include "GAS/CAttributeSet.h"
#include "Widgets/ValueGauge.h"

void UOverheadStatsGauge::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	if (WidthBox)
	{
		WidthBox->SetWidthOverride(DefaultWidth);
	}
	if (HealthBox)
	{
		HealthBox->SetHeightOverride(DefaultHealthHeight);
	}
	if (ManaBox)
	{
		ManaBox->SetHeightOverride(DefaultManaHeight);
	}
}

void UOverheadStatsGauge::ConfigureAbilitySystemComponent(UAbilitySystemComponent* AbilitySystemComponent) const
{
	if (AbilitySystemComponent)
	{
		HealthBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(AbilitySystemComponent, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
	}
}

void UOverheadStatsGauge::SetOverheadWidgetScale(const float Scale) const
{
	if (Scale > 0.f && Scale < 1.f)
	{
		WidthBox->SetWidthOverride(DefaultWidth * Scale);
		HealthBox->SetHeightOverride(DefaultHealthHeight * Scale);
		ManaBox->SetHeightOverride(DefaultManaHeight * Scale);
	}
}
