// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

/**
 * 
 */
UCLASS()
class ACAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACAIController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Perception")
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY(EditDefaultsOnly, Category = "Perception")
	float SightRadius = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "AI Behavior")
	FName TargetBlackboardKeyName = "Target";
	
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* TargetActor, struct FAIStimulus Stimulus);
	UFUNCTION()
	void TargetForgotten(AActor* ForgottenActor);
	
	UObject* GetCurrentTarget() const;
	void SetCurrentTarget(UObject* NewTarget);

	UPROPERTY()
	TArray<AActor*> TargetsInSight;

	UObject* FindNearestTargetInSight() const;
};
