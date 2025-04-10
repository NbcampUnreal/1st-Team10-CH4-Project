#include "AI/Character/AIBaseCharacter.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/Character/AIBossCharacter.h"
#include "AI/UI/Consts.h"
#include "AI/UI/HealthBarWidget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CSAttributeComponent.h"
#include "Components/CSCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSTypes/CSCharacterTypes.h"
#include "GameModes/CSGameModeBase.h"
#include "Navigation/PathFollowingComponent.h"
#include "Net/UnrealNetwork.h"

AAIBaseCharacter::AAIBaseCharacter()
	: WidgetComponenet(CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthValue")))
{
	PrimaryActorTick.bCanEverTick = true;

	if (WidgetComponenet)
	{
		WidgetComponenet->SetupAttachment(RootComponent);
		WidgetComponenet->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComponenet->SetRelativeLocation(defs::HealthBarZ);
		static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/Blueprints/AI/UI/BP_AIHealthBar"));
		if (WidgetClass.Succeeded())
		{
			WidgetComponenet->SetWidgetClass(WidgetClass.Class);
		}
	}

	AttributeComponent = CreateDefaultSubobject<UCSAttributeComponent>(TEXT("AttributeComponent"));
	CombatComponent = CreateDefaultSubobject<UCSCombatComponent>(TEXT("CombatComponent"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WidgetComponenet && AttributeComponent)
	{
		if (auto const Widget = Cast<UHealthBarWidget>(WidgetComponenet->GetUserWidgetObject()))
		{
			Widget->SetBarValuePercent(AttributeComponent->GetHealthPercent());
		}
	}
}

void AAIBaseCharacter::SetupPlayerInputComponent(class UInputComponent* AIInputComponent)
{
	Super::SetupPlayerInputComponent(AIInputComponent);
}

UBehaviorTree* AAIBaseCharacter::GetBehaviorTree() const { return Tree; }
APatrolPath* AAIBaseCharacter::GetPatrolPath() const { return PatrolPath; }

FName AAIBaseCharacter::GetfirstAttackName() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;

	if (CurrentTime - LastPunchTime > ComboResetCooldown)
	{
		CurrentPunchIndex = 0;
	}
	else
	{
		CurrentPunchIndex = (CurrentPunchIndex + 1) % 3;
	}

	LastPunchTime = CurrentTime;

	switch (CurrentPunchIndex)
	{
	case 0: return FName("Punch1");
	case 1: return FName("Punch2");
	case 2: return FName("Punch3");
	}
	
	return FName("Punch1");
}

FName AAIBaseCharacter::GetsecondAttackName() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;

	if (CurrentTime - LastKickTime > ComboResetCooldown)
	{
		CurrentKickIndex = 0;
	}
	else
	{
		CurrentKickIndex = (CurrentKickIndex + 1) % 3;
	}

	LastKickTime = CurrentTime;

	switch (CurrentKickIndex)
	{
	case 0: return FName("Kick1");
	case 1: return FName("Kick2");
	case 2: return FName("Kick3");
	}

	return FName("Kick1");
}

FName AAIBaseCharacter::GetLowComboAttackName() const
{
	return FName("Default");
}
FName AAIBaseCharacter::GetRangeComboAttackName() const
{
	return FName("Default");
}
FName AAIBaseCharacter::GetJumpName() const
{
	return FName("Default");
}
FName AAIBaseCharacter::GetCrouchName() const
{
	return FName("Default");
}

int AAIBaseCharacter::firstAttack_Implementation()
{
	AI_Attack(GetfirstAttackMontage(), GetfirstAttackName());
	return 1;
}
int AAIBaseCharacter::secondAttack_Implementation()
{
	AI_Attack(GetfirstAttackMontage(), GetsecondAttackName());
	return 1;
}
int AAIBaseCharacter::LowComboAttack_Implementation()
{
	AI_Attack(GetLowComboAttackMontage(), GetLowComboAttackName());
	return 1;
}
int AAIBaseCharacter::RangeComboAttack_Implementation()
{
	AI_Attack(GetRangeComboAttackMontage(), GetRangeComboAttackName());
	return 1;
}


int AAIBaseCharacter::AI_Attack(UAnimMontage* SelectedMontage, FName SectionName)
{
	CombatComponent->PerformAttack(SelectedMontage, SectionName);
	return 1;
}


void AAIBaseCharacter::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void AAIBaseCharacter::PlayHitReactMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName("IsHitReacting"), true);
			BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
			BB->SetValueAsBool(FName("ShouldJump"), false);
			BB->SetValueAsBool(FName("ShouldCrouch"), false);
			BB->SetValueAsBool(FName("ShouldRunAway"), false);
			BB->SetValueAsBool(FName("RangeCombo"), false);
			BB->SetValueAsBool(FName("IsBusy"), true);
		}
	}
	
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		TArray<FName> Sections = {
			FName("HitReact1"),
			FName("HitReact2"),
		};

		int32 Index = FMath::RandRange(0, Sections.Num() - 1);
		FName SectionName = Sections[Index];
		AnimInstance->Montage_JumpToSection(SectionName);
	}
	
	StopMovement();
	GetWorldTimerManager().SetTimer(HitReactTimerHandle, this, &AAIBaseCharacter::ResumeMovement, HitStunDuration, false);
}


void AAIBaseCharacter::ResumeMovement()
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
	GetWorldTimerManager().ClearTimer(HitReactTimerHandle);

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BB = AICon->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName("IsHitReacting"), false);
			BB->SetValueAsBool(FName("IsBusy"), false);
			
			const float BlockChance = 0.4f;
			if (FMath::FRand() < BlockChance)
			{
				BB->SetValueAsBool(FName("ShouldBlock"), true);
			}
		}
	}
}


void AAIBaseCharacter::Die()
{
	Super::Die();

	StopMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);

	AAIBaseCharacter* DeadAI = Cast<AAIBaseCharacter>(this);
	if (DeadAI)
	{
		if (ACSGameModeBase* GameMode = Cast<ACSGameModeBase>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->HandleAIDeath(DeadAI);
		}
	}
	
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
		AIController->UnPossess();
	}
	
	SetLifeSpan(5.0f);
}

int AAIBaseCharacter::Block_Implementation()
{
	if (!CombatComponent) return 0;
	if (bIsBlocking == true) return 0;
	bIsBlocking = true;
	if (!BlockMontage) return 0;
	if (BlockMontage)
	{
		PlayAnimMontage(BlockMontage);
		StopMovement();
	}
	return 1;
}

void AAIBaseCharacter::StopBlock()
{
	bIsBlocking = false;
	ResumeMovement();
	GetWorldTimerManager().ClearTimer(BlockTimerHandle);
}

int AAIBaseCharacter::Dodge_Implementation(AActor* Attacker)
{
	Dodge_StartDash(Attacker);
	return 1;
}

void AAIBaseCharacter::Dodge_StartDash(AActor* Attacker)
{
	if (!Attacker) return;
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();
	
	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector AwayDir = FVector(0.f, YDir, 0.f);
	
	float DashStrength = 900.f;
	LaunchCharacter(AwayDir * DashStrength, true, true);
	
	FTimerHandle DodgeMoveTimerHandle;
	GetWorldTimerManager().SetTimer(
		DodgeMoveTimerHandle,
		FTimerDelegate::CreateUObject(this, &AAIBossCharacter::Dodge_MoveToSafeZone, Attacker),
		0.25f, false
	);
}


void AAIBaseCharacter::Dodge_MoveToSafeZone(AActor* Attacker)
{
	ResumeMovement(); 
	if (!Attacker) return;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}
	
	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();
	
	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector RunDirection = FVector(0.f, YDir, 0.f);

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return;
	}
	
	FNavLocation NavLocation;

	const float MinDistance = 200.f;
	const float MaxDistance = 400.f;
	const int MaxAttempts = 10;
	for (int i = 0; i < MaxAttempts; ++i)
	{
		FVector BasePoint = MyLocation + RunDirection * MinDistance;

		if (NavSys->GetRandomReachablePointInRadius(BasePoint, MaxDistance, NavLocation))
		{
			float ActualDistance = FVector::Dist2D(MyLocation, NavLocation.Location);

			if (ActualDistance >= MinDistance)
			{
				NavLocation.Location.X = MyLocation.X;
				NavLocation.Location.Z = MyLocation.Z;
				break;
			}
		}
	}
	AIController->MoveToLocation(NavLocation.Location, -1.f, true);
}

int AAIBaseCharacter::RunAway_Implementation(AActor* Attacker)
{
	ResumeMovement();
	if (!Attacker) return 0;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return 0;
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();

	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector RunDirection = FVector(0.f, YDir, 0.f);

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return 0;

	FNavLocation NavLocation;

	const float MinDistance = 200.f;
	const float MaxDistance = 400.f;
	const int MaxAttempts = 10;
	bool bFoundValid = false;

	for (int i = 0; i < MaxAttempts; ++i)
	{
		FVector BasePoint = MyLocation + RunDirection * MinDistance;

		if (NavSys->GetRandomReachablePointInRadius(BasePoint, MaxDistance, NavLocation))
		{
			float ActualDistance = FVector::Dist2D(MyLocation, NavLocation.Location);
			if (ActualDistance >= MinDistance)
			{
				NavLocation.Location.X = MyLocation.X;
				NavLocation.Location.Z = MyLocation.Z;

				bFoundValid = true;
				break;
			}
		}
	}

	if (!bFoundValid)
	{
		return 0;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(NavLocation.Location);
	MoveRequest.SetAcceptanceRadius(5.f);
	AIController->MoveTo(MoveRequest);

	return 1;
}
