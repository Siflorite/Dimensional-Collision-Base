// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class UCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCGameplayAbility();

protected:
	class UAnimInstance* GetOwnerAnimInstance() const;

	TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const float SweepSphereRadius = 30.f, const ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, const bool bDrawDebugTrajectory = false, const bool bIgnoreSelf = true) const;
};
