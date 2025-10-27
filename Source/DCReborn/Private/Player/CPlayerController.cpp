// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/CPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "Player/CPlayerCharacter.h"
#include "Widgets/GameplayWidget.h"

// Server Init
void ACPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	if (CPlayerCharacter = Cast<ACPlayerCharacter>(NewPawn); CPlayerCharacter)
	{
		CPlayerCharacter->ServerSideInit();
		CPlayerCharacter->SetGenericTeamId(TeamID);
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

void ACPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACPlayerController::GetGenericTeamId() const
{
	return TeamID;
}

void ACPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACPlayerController, TeamID)
}


