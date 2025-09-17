// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifySendGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotifySendGameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                          const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* AbilityOwner = MeshComp->GetOwner())
	{
		// 检查目标AActor是否持有能力系统组件
		if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AbilityOwner)) return;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AbilityOwner, EventTag, FGameplayEventData());
	}
}

FString UAnimNotifySendGameplayEvent::GetNotifyName_Implementation() const
{
	// Event Tag的名称存在
	if (EventTag.IsValid())
	{
		// Example: Ability.Combo.Change.Combo_02 -> Combo_02
		// TArray<FName> TagNames;
		// UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
		// return TagNames.Last().ToString();
		return EventTag.GetTagLeafName().ToString();
		
	}
	return Super::GetNotifyName_Implementation();
}
