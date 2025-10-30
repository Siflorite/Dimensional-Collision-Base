// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CCharacter.h"

#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
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

	BindGASChangeDelegate();
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

void ACCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCharacter, TeamID)
}

// Called when the game starts or when spawned
void ACCharacter::BeginPlay()
{
	Super::BeginPlay();

	// UI Stuff
	ConfigureOverheadWidgetStatus();

	// Record relative transform of mesh to root component
	MeshRelativeTransform = GetMesh()->GetRelativeTransform();
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

void ACCharacter::BindGASChangeDelegate()
{
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->RegisterGameplayTagEvent(UCAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &ACCharacter::DeadTagUpdated);
	}
}

// ReSharper disable once CppParameterMayBeConst
void ACCharacter::DeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		// 当前有"Stats.Dead"标签，进入死亡状态
		StartDeathSequence();
	}
	else
	{
		// 当前"Stats.Dead"标签清除，进入复活流程
		Respawn();
	}
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

void ACCharacter::SetOverheadWidgetEnabled(const bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(OverheadWidgetTimerHandle);
	if (bIsEnabled)
	{
		ConfigureOverheadWidgetStatus();
	}
	else
	{
		OverheadWidgetComponent->SetHiddenInGame(true);
	}
}

void ACCharacter::StartDeathSequence()
{
	// 播放死亡动画
	PlayDeathAnimation();
	// 隐藏头顶组件
	SetOverheadWidgetEnabled(false);
	// 禁止移动
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	// 取消胶囊体碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    // 子类实现禁用输入等功能
	OnDead();
	// 等待死亡动画蒙太奇时间 + 偏移时间后，会调用DeathMontageFinished()，启用布娃娃系统
	// 包括人物模型脱离根组件、启用模拟物理、启用碰撞
}

void ACCharacter::Respawn()
{
	// 子类实现恢复输入等功能
	OnRespawn();
	// 关闭布娃娃系统，包括人物模型绑定到根组件，设置网格体与根组件的相对位置，关闭物理模拟，关闭碰撞。
	SetRagdollEnabled(false);
	// 恢复胶囊体碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// 恢复移动
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	// 显示头顶组件
	SetOverheadWidgetEnabled(true);
	// 停止死亡蒙太奇动画，淡出时间为0
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	// 玩家移动至出生点
	if (HasAuthority() && GetController())
	{
		if (const TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot; StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}
	// 恢复生命值、魔法等
	if (CAbilitySystemComponent)
	{
		CAbilitySystemComponent->ApplyFullStatEffect();
	}
	
}

// Empty in `ACCharacter`, implemented in children classes to be executed in `StartDeathSequence()`
void ACCharacter::OnDead()
{
}

// Empty in `ACCharacter`, implemented in children classes to be executed in `Respawn()`
void ACCharacter::OnRespawn()
{
}

void ACCharacter::SetRagdollEnabled(const bool bIsEnabled) const
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		// 必须要先关闭模拟物理，移动相对位置才能到正确的位置
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void ACCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		const float MontageDuration = PlayAnimMontage(DeathMontage);
		GetWorldTimerManager().SetTimer(
			DeathMontageTimerHandle,
			this,
			&ACCharacter::DeathMontageFinished,
			MontageDuration + DeathMontageFinishTimeShift
		);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void ACCharacter::DeathMontageFinished()
{
	SetRagdollEnabled(true);
}

void ACCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId ACCharacter::GetGenericTeamId() const
{
	return TeamID;
}
