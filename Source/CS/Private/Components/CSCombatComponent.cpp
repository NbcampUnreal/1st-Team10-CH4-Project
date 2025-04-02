#include "Components/CSCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/CSPlayerCharacter.h"

UCSCombatComponent::UCSCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicated(true);
}

void UCSCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCSCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCSCombatComponent, bIsAttacking);
}

void UCSCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCSCombatComponent::SetIsAttacking(bool bAttacking)
{
    if (GetOwner()->HasAuthority())
    {
        bool bOldValue = bIsAttacking;
        bIsAttacking = bAttacking;

        if (bOldValue != bIsAttacking)
        {
            OnRep_IsAttacking();
        }
    }
}

void UCSCombatComponent::OnRep_IsAttacking()
{
    ACSPlayerCharacter* Character = Cast<ACSPlayerCharacter>(GetOwner());
    if (Character)
    {
        if (bIsAttacking)
        {
            Character->PlayAttackMontage();
        }
    }
}

void UCSCombatComponent::ServerStartAttack_Implementation()
{
    SetIsAttacking(true);
}

void UCSCombatComponent::ServerEndAttack_Implementation()
{
    SetIsAttacking(false);
}