#include "AI/Character/AIBaseCharacter.h"
#include "AIController.h"
#include "AI/Character/AIBossCharacter.h"
#include "AI/UI/Consts.h"
#include "AI/UI/HealthBarWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/CSAttributeComponent.h"
#include "Components/CSCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSTypes/CSCharacterTypes.h"
#include "GameModes/CSGameModeBase.h"
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

FName AAIBaseCharacter::GetPunchMontage() const
{
	TArray<FName> Sections = {
		FName("Punch1"),
		FName("Punch2"),
		FName("Punch3")
	};

	int32 Index = FMath::RandRange(0, Sections.Num() - 1);
	return Sections[Index];
}
FName AAIBaseCharacter::GetJumpMontage() const
{
	return FName("Default");
}
FName AAIBaseCharacter::GetCrouchMontage() const
{
	return FName("Default");
}

int AAIBaseCharacter::MeleeAttack_Implementation()
{
	if (!CombatComponent) return 0;

	UAnimMontage* SelectedMontage = nullptr;
	FName SectionName = NAME_None;

	switch (CurrentStance)
	{
	case ECharacterStance::Standing:
		SelectedMontage = GetAttackMontage();
		SectionName = GetPunchMontage();
		break;

	case ECharacterStance::Jumping:
		SelectedMontage = GetJumpAttackMontage();
		SectionName = GetJumpMontage();
		break;

	case ECharacterStance::Crouching:
		SelectedMontage = GetCrouchAttackMontage();
		SectionName = GetCrouchMontage();
		break;

	default:
		return 0;
	}

	if (!SelectedMontage) return 0;

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

	
	if (AnimInstance && HitReactMontage)
	{
		
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName = "HitReact1";
		AnimInstance->Montage_JumpToSection(SectionName);
	}
	
	UpdateLastDamageTime(); 


	UE_LOG(LogTemp, Warning, TEXT("HitReact called. Damage time updated: %.2f"), GetWorld()->TimeSeconds);


	StopMovement();
	
	GetWorldTimerManager().SetTimer(HitReactTimerHandle, this, &AAIBaseCharacter::ResumeMovement, HitStunDuration, false);
}


void AAIBaseCharacter::ResumeMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetWorldTimerManager().ClearTimer(HitReactTimerHandle);
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

void AAIBaseCharacter::StopBlock()
{
}

void AAIBaseCharacter::UpdateLastDamageTime()
{
	LastDamageTime = GetWorld()->TimeSeconds;
}


bool AAIBaseCharacter::WasRecentlyDamaged(float DamageTimeout) const
{
	return GetWorld()->TimeSeconds - LastDamageTime < DamageTimeout;
}