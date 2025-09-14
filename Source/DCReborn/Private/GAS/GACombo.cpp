// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/GACombo.h"

void UGACombo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// CommitAbility() 会检测当前是否满足技能释放条件，如冷却、蓝量等
	// 如果满足，则会消耗对应资源，并返回true；若不满足，则不消耗，返回false
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	UE_LOG(LogTemp, Warning, TEXT("Activating GACombo"));
}
