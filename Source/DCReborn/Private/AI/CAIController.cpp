// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SightRadius = SightRadius;
	SightConfig->LoseSightRadius = SightRadius + 200.f;

	SightConfig->SetMaxAge(5.f); // 目标走出LoseSightRadius距离后仍然索敌的时间

	SightConfig->PeripheralVisionAngleDegrees = 180.f; // 前向量正负180度

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::TargetPerceptionUpdated);

	TargetsInSight.Empty();
}

// Only executed on server side
void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	// 设置Controller的TeamId
	SetGenericTeamId(FGenericTeamId(0)); // Temporarily use TeamId = 0 <=> Team01

	// 设置角色（一般是ACCharacter）的TeamId
	if (IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();
	if (BehaviorTree)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

// ReSharper disable once CppPassValueParameterByConstReference
void ACAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed()) // 看到新目标
	{
		TargetsInSight.AddUnique(TargetActor);
		if (!GetCurrentTarget()) // 当前无追踪目标，设置目标为看到的Actor
		{
			SetCurrentTarget(TargetActor);
		}
		// 有追踪目标时保持追踪目标
	}
	else // 有玩家离开视野
	{
		TargetsInSight.Remove(TargetActor);
		if (TargetsInSight.IsEmpty())
		{
			SetCurrentTarget(nullptr);
		}
		else
		{
			SetCurrentTarget(FindNearestTargetInSight());
		}
	}
}

UObject* ACAIController::GetCurrentTarget() const
{
	if (const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		return BlackboardComponent->GetValueAsObject(TargetBlackboardKeyName);
	}
	return nullptr;
}

void ACAIController::SetCurrentTarget(UObject* NewTarget)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (NewTarget)
		{
			BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
		}
		else
		{
			BlackboardComponent->ClearValue(TargetBlackboardKeyName);
		}
	}
}

UObject* ACAIController::FindNearestTargetInSight() const
{
	if (!TargetsInSight.IsEmpty())
	{
		if (const APawn* SelfPawn = GetPawn())
		{
			const FVector SelfLocation = SelfPawn->GetActorLocation();
			UObject* NearestTarget = nullptr;
			float NearestDistance = SightRadius;

			for (AActor* TargetActor : TargetsInSight)
			{
				if (!TargetActor) continue;

				if (const float Distance = FVector::Distance(SelfLocation, TargetActor->GetActorLocation()); Distance < NearestDistance)
				{
					NearestDistance = Distance;
					NearestTarget = TargetActor;
				}
			}
			return NearestTarget;
		}
	}
	return nullptr;
}
