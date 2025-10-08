// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemInterface.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CCharacter.generated.h"

UCLASS()
class ACCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACCharacter();
	// 服务端与客户端初始化
	void ServerSideInit();
	void ClientSideInit();
	// 只会在服务端被调用
	virtual void PossessedBy(AController* NewController) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
// ---------------------------  Gameplay Ability System  ----------------------------------
	
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	void BindGASChangeDelegate();

	void DeadTagUpdated(const struct FGameplayTag Tag, int32 NewCount);
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UCAbilitySystemComponent* CAbilitySystemComponent;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UCAttributeSet* CAttributeSet;

// ---------------------------------  UI Components  --------------------------------------
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	class UWidgetComponent* OverheadWidgetComponent;

	void ConfigureOverheadWidgetStatus();

	// 避免平方根
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float OverheadWidgetVisibilityRangeSquared = 9000000.f; // 30 meters

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float MinDistSquared = 250000.f; // 5 meters
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float OverheadWidgetTimerInterval = 0.01f; // 10 ms
	
	FTimerHandle OverheadWidgetTimerHandle;
	void UpdateOverheadWidget() const;

// --------------------------------  Death and Respawn  -----------------------------------

private:
	void StartDeathSequence();
	void Respawn();
};
