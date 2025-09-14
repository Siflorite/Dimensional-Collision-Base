// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	// Ensure the initialization effects only commence on servers!
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	// Ensure the initialization effects only commence on servers!
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass: Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityClass, 0, -1, nullptr));
	}

	for (const TSubclassOf<UGameplayAbility>& AbilityClass: BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, -1, nullptr));
	}
}
