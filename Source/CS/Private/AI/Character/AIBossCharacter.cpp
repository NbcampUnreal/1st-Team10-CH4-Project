#include "AI/Character/AIBossCharacter.h"
#include "AI/Controller/AIBossController.h"
#include "Components/CSCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AAIBossCharacter::AAIBossCharacter()
{
	AIControllerClass = AAIBossController::StaticClass();
}

void AAIBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentStance = ECharacterStance::Standing;
}

void AAIBossCharacter::JumpAction()
{
		Jump();
		CurrentStance = ECharacterStance::Jumping;

		
		FTimerHandle JumpEndHandle;
		GetWorldTimerManager().SetTimer(JumpEndHandle, this, &AAIBossCharacter::EndJump, 1.0f, false);
	
}

void AAIBossCharacter::EndJump()
{
	CurrentStance = ECharacterStance::Standing;
}

void AAIBossCharacter::CrouchAction()
{
	Crouch();
	CurrentStance = ECharacterStance::Crouching;


	FTimerHandle CrouchEndHandle;
	GetWorldTimerManager().SetTimer(CrouchEndHandle, this, &AAIBossCharacter::EndCrouch, 1.0f, false);
}

void AAIBossCharacter::EndCrouch()
{
	UnCrouch(); 
	CurrentStance = ECharacterStance::Standing;
}


void AAIBossCharacter::StartComboAttack()
{
	if (!CombatComponent) return;

	TArray<FName> ComboSections = { "Punch1", "Punch2", "Punch3" };
	if (CurrentComboIndex >= ComboSections.Num())
	{
		CurrentComboIndex = 0;
	}

	UAnimMontage* Montage = GetAttackMontage();
	if (Montage)
	{
		CombatComponent->PerformAttack(Montage, ComboSections[CurrentComboIndex]);
		CurrentComboIndex++;
		GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this, &AAIBossCharacter::ResetCombo, 2.f);
	}
}

void AAIBossCharacter::ResetCombo()
{
	CurrentComboIndex = 0;
}

int AAIBossCharacter::Block_Implementation()
{
	if (!CombatComponent) return 0;
	if (bIsBlocking == true) return 0;
	bIsBlocking = true;
	if (!BlockMontage) return 0;
	if (BlockMontage)
	{
		PlayAnimMontage(BlockMontage);
		
		UE_LOG(LogTemp, Warning, TEXT("Block Montage:"));
	}
	return 1;
}

void AAIBossCharacter::StopBlock()
{
	bIsBlocking = false;
	
	GetWorldTimerManager().ClearTimer(BlockTimerHandle);
}



