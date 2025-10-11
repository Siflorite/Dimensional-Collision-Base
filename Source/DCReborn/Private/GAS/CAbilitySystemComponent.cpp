// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
	GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &UCAbilitySystemComponent::HealthUpdated);
}

/// @brief Apply Gameplay Effect to player, only called on server.
/// @param GameplayEffect  The effect class to be applied.
/// @param Level  The level of the effect to be applied.
void UCAbilitySystemComponent::AuthApplyGameplayEffect(const TSubclassOf<UGameplayEffect>& GameplayEffect, const int Level)
{
	// Ensure executed on server
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
	// Ensure the initialization of effects only commence on servers!
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		const FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
	// Ensure the initialization of abilities only commence on servers!
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair: Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, static_cast<int32>(AbilityPair.Key), nullptr));
	}

	for (const TPair<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair: BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, static_cast<int32>(AbilityPair.Key), nullptr));
	}
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
	if (FullStatEffect)
	{
		AuthApplyGameplayEffect(FullStatEffect);
	}
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& Data)
{
	// 新的生命值小于等于0，赋予死亡效果
	if (Data.NewValue <= 0 && DeathEffect)
	{
		AuthApplyGameplayEffect(DeathEffect);
	}
}
