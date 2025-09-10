// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "CAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class UCAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	// 使用宏自动创建基础的Init, Setter, Getter等
	ATTRIBUTE_ACCESSORS_BASIC(UCAttributeSet, Health)
	ATTRIBUTE_ACCESSORS_BASIC(UCAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS_BASIC(UCAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS_BASIC(UCAttributeSet, MaxMana)
	// 注册需要复制的变量
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	private:
		UPROPERTY(ReplicatedUsing = OnRep_Health)
		FGameplayAttributeData Health;
		UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
		FGameplayAttributeData MaxHealth;
		UPROPERTY(ReplicatedUsing = OnRep_Mana)
		FGameplayAttributeData Mana;
		UPROPERTY(ReplicatedUsing = OnRep_MaxMana)
		FGameplayAttributeData MaxMana;

		// 网络复制回调函数
		UFUNCTION()
		void OnRep_Health(const FGameplayAttributeData& OldValue) const;
		UFUNCTION()
		void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) const;
		UFUNCTION()
		void OnRep_Mana(const FGameplayAttributeData& OldValue) const;
		UFUNCTION()
		void OnRep_MaxMana(const FGameplayAttributeData& OldValue) const;
};
