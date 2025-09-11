// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerController.h"

#include "Player/CPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"

// Server Init
void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	if (CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn); CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
	}
}

// Client Init
void ACPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	if (CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn); CPlayerCharacter)
	{
		CPlayerCharacter->ClientSideInit();
		SpawnGameplayWidget();
	}
}

void ACPlayerController::SpawnGameplayWidget()
{
	if (!IsLocalPlayerController()) return;

	if (GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass); GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}
