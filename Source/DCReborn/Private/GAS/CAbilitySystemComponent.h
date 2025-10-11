// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "CAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class UCAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UCAbilitySystemComponent();
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();

	// 完全恢复生命值、魔法值
	void ApplyFullStatEffect();

private:
	void AuthApplyGameplayEffect(const TSubclassOf<UGameplayEffect>& GameplayEffect, const int Level = 1);
	
	void HealthUpdated(const FOnAttributeChangeData& Data);
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
	TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;
};
