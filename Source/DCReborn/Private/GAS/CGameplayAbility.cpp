// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/CGameplayAbility.h"
#include "Kismet/KismetSystemLibrary.h"

UCGameplayAbility::UCGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UAnimInstance* UCGameplayAbility::GetOwnerAnimInstance() const
{
	if (const USkeletalMeshComponent* OwnerSkeletalMeshComponent = GetOwningComponentFromActorInfo())
	{
		return OwnerSkeletalMeshComponent->GetAnimInstance();
	}
	return nullptr;
}

TArray<FHitResult> UCGameplayAbility::GetHitResultsFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle,
	const float SweepSphereRadius,
	const ETeamAttitude::Type TargetTeam,
	const bool bDrawDebugTrajectory,
	const bool bIgnoreSelf) const
{
	TArray<FHitResult> OutHitResults;
	TSet<AActor*> HitActors;

	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());

	for (const TSharedPtr<FGameplayAbilityTargetData>& TargetData : TargetDataHandle.Data)
	{
		const FVector StartLocation = TargetData->GetOrigin().GetLocation();
		const FVector EndLocation = TargetData->GetEndPoint();

		TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
		HitObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf)
		{
			ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
		}

		const EDrawDebugTrace::Type DrawDebugTraceType = bDrawDebugTrajectory ?
			EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		TArray<FHitResult> OutHits;
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLocation, EndLocation, SweepSphereRadius,
			HitObjectTypes, false, ActorsToIgnore, DrawDebugTraceType, OutHits, false);
		// OutHitResults.Append(OutHits);

		for (const FHitResult& HitResult : OutHits)
		{
			// 包含了重复Actor
			if (HitActors.Contains(HitResult.GetActor())) continue;
			// 目标Actor与自身不是Hostile
			if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*HitResult.GetActor()) != TargetTeam) continue;

			OutHitResults.Add(HitResult);
			HitActors.Add(HitResult.GetActor());
		}
	}

	return OutHitResults;
}
