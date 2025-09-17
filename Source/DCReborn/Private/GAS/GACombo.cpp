// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GACombo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "GAS/CAbilitySystemStatics.h"

UGACombo::UGACombo()
{
	// `AbilityTags` might be deprecated after UE 5.6,
	// use `GetAssetTags()` and `SetAssetTags()` to replace `AbilityTags`
	// AbilityTags.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
	const FGameplayTagContainer Tags = FGameplayTagContainer(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
	SetAssetTags(Tags);
	BlockAbilitiesWithTag.AddTag(UCAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGACombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// CommitAbility() 会检测当前是否满足技能释放条件，如冷却、蓝量等
	// 如果满足，则会消耗对应资源，并返回true；若不满足，则不消耗，返回false
	// if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	// {
	// 	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	// 	return;
	// }

	// K2版本的蓝图函数不需要参数，直接内部检查，更简练
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &UGACombo::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &UGACombo::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &UGACombo::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &UGACombo::K2_EndAbility);
		PlayComboMontageTask->ReadyForActivation();

		UAbilityTask_WaitGameplayEvent* WaitComboChangeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, GetComboChangedEventTag(), nullptr, false, false);
		WaitComboChangeEventTask->EventReceived.AddDynamic(this, &UGACombo::HandleComboChangedEvent);
		WaitComboChangeEventTask->ReadyForActivation();
		
		SetupInputPress();
	}
}

FGameplayTag UGACombo::GetComboChangedEventTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change");
}

FGameplayTag UGACombo::GetComboChangedEventEndTag()
{
	return FGameplayTag::RequestGameplayTag("Ability.Combo.Change.End");
}

void UGACombo::SetupInputPress()
{
	// Hard-coded to be triggered only once
	UAbilityTask_WaitInputPress* WaitInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPressTask->OnPress.AddDynamic(this, &UGACombo::HandleInputPress);
	WaitInputPressTask->ReadyForActivation();
}

// Delegate functions in `EventReceived.AddDynamic` require a whole `FGameplayEventData` struct passing in, while reference is not allowed.
// ReSharper disable once CppPassValueParameterByConstReference
void UGACombo::HandleComboChangedEvent(FGameplayEventData Data)
{
	if (const FGameplayTag& EventTag = Data.EventTag; EventTag == GetComboChangedEventEndTag())
	{
		NextComboName = NAME_None;
		// UE_LOG(LogTemp, Warning, TEXT("Next Combo is cleared."));
	}
	else
	{
		NextComboName = EventTag.GetTagLeafName();
		// UE_LOG(LogTemp, Warning, TEXT("Next Combo Name: %s"), *NextComboName.ToString());
	}
}

void UGACombo::HandleInputPress(float TimeWaited)
{
	SetupInputPress();
	
	if (NextComboName == NAME_None) return;
	if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
	{
		OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
	}
}
