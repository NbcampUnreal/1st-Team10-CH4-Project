#include "AI/Character/AINormalCharacter.h"
#include "AI/Controller/AINormalController.h"
#include "Components/CSCombatComponent.h"

AAINormalCharacter::AAINormalCharacter()
{
	AIControllerClass = AAINormalController::StaticClass();
	Tags.Add(FName("Normal"));
}

FComboAttackData AAINormalCharacter::GetFirstAttackData() const
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

	FComboAttackData AttackData;

	switch (CurrentPunchIndex)
	{
	case 0:
		AttackData.SectionName = FName("Punch1");
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	case 1:
		AttackData.SectionName = FName("Punch2");
		AttackData.Damage = 12.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	case 2:
		AttackData.SectionName = FName("Punch3");
		AttackData.Damage = 15.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	default:
		AttackData.SectionName = FName("Punch1");
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	}

	return AttackData;
}

int AAINormalCharacter::AI_Attack(UAnimMontage* SelectedMontage, const FComboAttackData& AttackData)
{
	CombatComponent->MultiSetMontageData(SelectedMontage, AttackData.SectionName);
	
	FName TraceStart = FName("RightFistSocket");
	FName TraceEnd = FName("RightFistSocketEnd");

	CombatComponent->Server_PerformHitCheck(TraceStart, TraceEnd, AttackData.Damage, AttackData.DType);

	return 1;
}