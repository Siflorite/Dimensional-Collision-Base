// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GACombo.generated.h"

/**
 * 
 */
UCLASS()
class UGACombo : public UCGameplayAbility
{
	GENERATED_BODY()

public:
	UGACombo();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ComboMontage;

	void SetupInputPress();
	
	UFUNCTION()
	void HandleComboChangedEvent(FGameplayEventData Data);
	UFUNCTION()
	void HandleInputPress(float TimeWaited);

	static FGameplayTag GetComboChangedEventTag();
	static FGameplayTag GetComboChangedEventEndTag();
	FName NextComboName;
};
