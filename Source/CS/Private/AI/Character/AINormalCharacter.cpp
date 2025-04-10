#include "AI/Character/AINormalCharacter.h"
#include "AI/Controller/AINormalController.h"
#include "Components/CSCombatComponent.h"

AAINormalCharacter::AAINormalCharacter()
{
	AIControllerClass = AAINormalController::StaticClass();
	Tags.Add(FName("Normal"));
}

FName AAINormalCharacter::GetPunchName() const
{
	const int32 RandomIndex = FMath::RandRange(0, 2);

	switch (RandomIndex)
	{
	case 0: return FName("Punch1");
	case 1: return FName("Punch2");
	case 2: return FName("Punch3");
	}

	return FName("Punch1");
}

int AAINormalCharacter::AI_Attack(UAnimMontage* SelectedMontage, FName SectionName)
{
	CombatComponent->PerformAttack(SelectedMontage, SectionName);
	return 1;
}