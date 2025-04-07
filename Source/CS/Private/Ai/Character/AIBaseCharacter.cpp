#include "AI/Character/AIBaseCharacter.h"
#include "AIController.h"
#include "AI/UI/Consts.h"
#include "AI/UI/HealthBarWidget.h"
#include "Components/CSAttributeComponent.h"
#include "Components/CSCombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSTypes/CSCharacterTypes.h"
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

FName AAIBaseCharacter::GetMontage() const
{
	TArray<FName> Sections = {
		FName("Punch1"),
		FName("Punch2"),
		FName("Punch3")
	};

	int32 Index = FMath::RandRange(0, Sections.Num() - 1);
	return Sections[Index];
}

int AAIBaseCharacter::MeleeAttack_Implementation()
{
	if (!CombatComponent) return 0;
	UAnimMontage* SelectedMontage = GetAttackMontage();
	FName SectionName = GetMontage();
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
	
	StopMovement();
	
	GetWorldTimerManager().SetTimer(HitReactTimerHandle, this, &AAIBaseCharacter::ResumeMovement, HitStunDuration, false);
}

void AAIBaseCharacter::ResumeMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	GetWorldTimerManager().ClearTimer(HitReactTimerHandle);
}