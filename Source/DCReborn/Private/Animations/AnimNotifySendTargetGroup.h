// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotifySendTargetGroup.generated.h"

/**
 * 
 */
UCLASS()
class UAnimNotifySendTargetGroup : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
	FGameplayTag EventTag;
	
	UPROPERTY(EditAnywhere, Category = "Gameplay Abilities")
	TArray<FName> TargetSocketNames;
};
