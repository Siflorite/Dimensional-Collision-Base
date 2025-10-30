// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CGameMode.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

APlayerController* ACGameMode::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	const FGenericTeamId TeamId = GetTeamIdForPlayer(NewPlayerController);
	
	if (IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController))
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}
	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamId);
	
	return NewPlayerController;
}

FGenericTeamId ACGameMode::GetTeamIdForPlayer(const APlayerController* PlayerController) const
{
	static uint8 PlayerCount = 0;
	PlayerCount++;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* ACGameMode::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	if (const FName* PlayerStart = TeamStartSpotTagMap.Find(TeamID)) // Find()返回的是const ValueType*指针
	{
		const UWorld* World = GetWorld();
		for (TActorIterator<APlayerStart> It(World); It; ++It)
		{
			if (It->PlayerStartTag == *PlayerStart)
			{
				It->PlayerStartTag = FName("Taken"); // 避免两个角色占用相同的出生点
				return *It;
			}
		}
	}

	// 找不到TeamID或对应TeamID Tag的出生点
	return nullptr;
}
