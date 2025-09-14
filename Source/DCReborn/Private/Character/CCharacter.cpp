// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/OverheadStatsGauge.h"

// Sets default values
ACCharacter::ACCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// GAS Initialization
	CAbilitySystemComponent = CreateDefaultSubobject<UCAbilitySystemComponent>("CAbility System Component");
	CAttributeSet = CreateDefaultSubobject<UCAttributeSet>("CAttribute Set");

	// UI Initialization
	OverheadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Overhead Widget Component");
	OverheadWidgetComponent->SetupAttachment(GetRootComponent());
}

void ACCharacter::ServerSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
	// 服务端初始化
	CAbilitySystemComponent->ApplyInitialEffects();
	CAbilitySystemComponent->GiveInitialAbilities();
}

void ACCharacter::ClientSideInit()
{
	CAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void ACCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (NewController && !NewController->IsPlayerController())
	{
		// AI
		ServerSideInit();
	}
}

// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	// UI Stuff
	ConfigureOverheadWidgetStatus();
}

// Called every frame
void ACCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* ACCharacter::GetAbilitySystemComponent() const
{
	return CAbilitySystemComponent;
	// return Cast<UAbilitySystemComponent>(CAbilitySystemComponent);
}

void ACCharacter::ConfigureOverheadWidgetStatus()
{
	if (!OverheadWidgetComponent) return;

	if (IsLocallyControlled())
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
		return;
	}
	
	if (const UOverheadStatsGauge* OverheadStatsGauge = Cast<UOverheadStatsGauge>(OverheadWidgetComponent->GetUserWidgetObject()))
	{
		OverheadWidgetComponent->SetHiddenInGame(false);
		OverheadStatsGauge->ConfigureAbilitySystemComponent(GetAbilitySystemComponent());

		GetWorldTimerManager().ClearTimer(OverheadWidgetTimerHandle);
		GetWorldTimerManager().SetTimer(OverheadWidgetTimerHandle, this, &ACCharacter::UpdateOverheadWidget, OverheadWidgetTimerInterval, true);
	}
}

void ACCharacter::UpdateOverheadWidget() const
{
	if (const APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0))
	{
		const float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverheadWidgetComponent->SetHiddenInGame(DistSquared > OverheadWidgetVisibilityRangeSquared);
		
		// 头顶血条缩放
		if (DistSquared <= OverheadWidgetVisibilityRangeSquared)
		{
			const float DistClamped = FMath::Clamp(DistSquared, MinDistSquared, OverheadWidgetVisibilityRangeSquared);
			const float SizeRatio = FMath::Pow((MinDistSquared / DistClamped), 0.1f);
			const UOverheadStatsGauge* OverheadStatsGauge = Cast<UOverheadStatsGauge>(OverheadWidgetComponent->GetUserWidgetObject());
			OverheadStatsGauge->SetOverheadWidgetScale(SizeRatio);
		}
	}
}

