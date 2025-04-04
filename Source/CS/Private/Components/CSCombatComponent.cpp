#include "Components/CSCombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/CSPlayerCharacter.h"

UCSCombatComponent::UCSCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicated(true);

    // Reset Combo Data
    iCombo_1_Cnt = 0;
    iCombo_2_Cnt = 0;
    bCanCombo = true;
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

void UCSCombatComponent::Combo1CntIncrease()
{
    iCombo_1_Cnt++;
}

int32 UCSCombatComponent::GetCombo1Cnt()
{
    return iCombo_1_Cnt;
}

void UCSCombatComponent::Combo2CntIncrease()
{
    iCombo_2_Cnt++;
}

int32 UCSCombatComponent::GetCombo2Cnt()
{
    return iCombo_2_Cnt;
}

void UCSCombatComponent::CanComboChange(bool Check)
{
    bCanCombo = Check;
}

bool UCSCombatComponent::GetCanCombo()
{
    return bCanCombo;
}

void UCSCombatComponent::ResetComboData()
{
    iCombo_1_Cnt = 0;
    iCombo_2_Cnt = 0;
    bCanCombo = true;
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

void UCSCombatComponent::MultiSetMontageData_Implementation(UAnimMontage* PlayMontage, FName Section)
{
    ServerPlayMontage = PlayMontage;
    ServerSection = Section;
}

void UCSCombatComponent::ServerSetMontageData_Implementation(UAnimMontage* PlayMontage, FName Section)
{
    MultiSetMontageData(PlayMontage, Section);
    ServerStartAttack();
}

void UCSCombatComponent::OnRep_IsAttacking()
{
    ACSPlayerCharacter* Character = Cast<ACSPlayerCharacter>(GetOwner());
    if (Character)
    {
        if (bIsAttacking)
        {
            Character->PlayPlayerMontage(ServerPlayMontage, ServerSection);
            Character->StopMovement();
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