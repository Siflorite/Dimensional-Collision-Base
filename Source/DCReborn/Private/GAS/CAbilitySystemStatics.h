// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CAbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class UCAbilitySystemStatics: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static struct FGameplayTag GetBasicAttackAbilityTag();
};
