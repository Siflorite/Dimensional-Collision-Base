// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/CAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

ACAIController::ACAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;

	SightConfig->SetMaxAge(5.f); // 目标走出LoseSightRadius距离后仍然索敌的时间

	SightConfig->PeripheralVisionAngleDegrees = 180.f; // 前向量正负180度

	AIPerceptionComponent->ConfigureSense(*SightConfig);
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	SetGenericTeamId(FGenericTeamId(0));

	if (IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		PawnTeamInterface->SetGenericTeamId(GetGenericTeamId());
	}
}
