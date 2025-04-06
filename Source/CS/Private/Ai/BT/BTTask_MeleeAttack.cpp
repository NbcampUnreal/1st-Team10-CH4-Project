#include "AI/BT/BTTask_MeleeAttack.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack()
{
	NodeName = TEXT("Melee Attack");
}
EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const OutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	if (OutOfRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	
	auto const * const cont = OwnerComp.GetAIOwner();
	auto* const npc = Cast<AAIBaseCharacter>(cont->GetPawn());

	if (auto* const ICombat = Cast<ICombatInterface>(npc))
	{
		if (MontageHasfinished(npc))
		{
			ICombat->Execute_MeleeAttack(npc);
			
			npc->GetWorldTimerManager().SetTimer(
				AttackCooldownTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_MeleeAttack::ResetCanAttack, &OwnerComp),
				1.0f, false
			);
			
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_MeleeAttack::ResetCanAttack(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}


bool UBTTask_MeleeAttack::MontageHasfinished(AAIBaseCharacter * const AI)
{
	return AI->GetMesh()->GetAnimInstance()->Montage_GetIsStopped(AI->GetMontage());
}