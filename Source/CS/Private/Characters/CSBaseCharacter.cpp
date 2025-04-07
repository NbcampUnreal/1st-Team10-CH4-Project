// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CSBaseCharacter.h"
#include "GameModes/CSGameModeBase.h"
#include "Components/CSAttributeComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

ACSBaseCharacter::ACSBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACSBaseCharacter::Attack()
{
}

void ACSBaseCharacter::Die()
{
    if (HasAuthority())
    {
        ActionState = ECharacterTypes::ECT_Dead;
        OnRep_ActionState();

        GetCharacterMovement()->StopMovementImmediately();
        GetCharacterMovement()->DisableMovement();
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        Multicast_PlayDeathMontage();

        AGameModeBase* GM = GetWorld()->GetAuthGameMode();
        ACSGameModeBase* CurrentGameMode = Cast<ACSGameModeBase>(GM);
        AController* MyController = GetController();

        if (CurrentGameMode && MyController)
        {
            CurrentGameMode->HandlePlayerDeath(MyController);
        }

        SetLifeSpan(5.0f);
        
    }
}

void ACSBaseCharacter::PlayHitReactMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName = "HitReact1";
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ACSBaseCharacter::Multicast_PlayDeathMontage_Implementation()
{
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
    }
}

bool ACSBaseCharacter::IsAlive()
{
    return Attributes && Attributes->IsAlive();
}

void ACSBaseCharacter::ActivateSuddenDeath()
{
	// TODO: Damage Increase
}

bool ACSBaseCharacter::CanAttack()
{
	return false;
}

void ACSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void ACSBaseCharacter::PlayHitReactMontage()
{
}

void ACSBaseCharacter::PlayHitReactMontage_Implementation()
{
}

void ACSBaseCharacter::StopMovement()
{
}

void ACSBaseCharacter::StopMovement_Implementation()
{
}

void ACSBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACSBaseCharacter, ActionState);
}


void ACSBaseCharacter::OnRep_ActionState()
{
    switch (ActionState)
    {
    case ECharacterTypes::ECT_Dead:
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        GetCharacterMovement()->DisableMovement();
        break;
    case ECharacterTypes::ECT_Unoccupied:
        GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        GetCharacterMovement()->MaxWalkSpeed = 600.f;
        break;
    default:
        break;
    }
}

void ACSBaseCharacter::SetupStimulusSource()
{
    StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
    if (StimulusSource)
    {
        StimulusSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
        StimulusSource->RegisterWithPerceptionSystem();
    }
}