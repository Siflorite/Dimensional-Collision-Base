// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/AnimNotifySendTargetGroup.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAnimNotifySendTargetGroup::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	// 近战攻击路径扫描至少要有两个点
	if (TargetSocketNames.Num() <= 1) return;

	if (AActor* AbilityOwner = MeshComp->GetOwner())
	{
		// 检查目标AActor是否持有能力系统组件
		if (!UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AbilityOwner)) return;

		FGameplayEventData LocationData;
		// I hate this personally but there's no alternative like std::ranges::views::adjacent in C++ 23 or slice::windows in Rust
		// for TArray to make a window iteration, so just make sure you check everything...
		for (int32 i = 1; i < TargetSocketNames.Num(); i++)
		{
			// Heap Allocation, required by `Add()`
			FGameplayAbilityTargetData_LocationInfo* LocationInfo = new FGameplayAbilityTargetData_LocationInfo();
			
			FVector StartLocation = MeshComp->GetSocketLocation(TargetSocketNames[i-1]);
			FVector EndLocation = MeshComp->GetSocketLocation(TargetSocketNames[i]);
			
			LocationInfo->SourceLocation.LiteralTransform.SetLocation(StartLocation);
			LocationInfo->TargetLocation.LiteralTransform.SetLocation(EndLocation);
			
			// Add使用TSharedPtr包装输入参数DataPtr，且文档要求TargetData必须使用new创建，避免破坏RC计数
			LocationData.TargetData.Add(LocationInfo);
		}
		
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(AbilityOwner, EventTag, MoveTemp(LocationData));
	}
}
