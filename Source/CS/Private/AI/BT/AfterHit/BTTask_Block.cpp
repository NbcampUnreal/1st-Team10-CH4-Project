// UBTTask_Block.cpp

#include "AI/BT/AfterHit/BTTask_Block.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Block::UBTTask_Block()
{
	NodeName = TEXT("Block and Counter");
	ShouldBlockKey.SelectedKeyName = "ShouldBlock";
	IsPlayerAttackingKey.SelectedKeyName = "IsPlayerAttacking";
}

EBTNodeResult::Type UBTTask_Block::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const bool bShouldBlock = OwnerComp.GetBlackboardComponent()->GetValueAsBool(ShouldBlockKey.SelectedKeyName);
	const bool bIsPlayerAttacking = OwnerComp.GetBlackboardComponent()->GetValueAsBool(IsPlayerAttackingKey.SelectedKeyName);

	
	if (!(bShouldBlock || bIsPlayerAttacking))
	{
		return EBTNodeResult::Failed;
	}

	auto* AIController = OwnerComp.GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());

	if (auto* ICombat = Cast<ICombatInterface>(NPC))
	{
		if (ICombat->Execute_Block(NPC))
		{
		
			NPC->GetWorldTimerManager().SetTimer(
				BlockTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_Block::FinishBlock, &OwnerComp),
				1.5f, false
			);

			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_Block::FinishBlock(UBehaviorTreeComponent* OwnerComp)
{
	auto* AIController = OwnerComp->GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());

	if (auto* ICombat = Cast<ICombatInterface>(NPC))
	{
		NPC->StopBlock();
		ICombat->Execute_MeleeAttack(NPC); 
	}


	auto* BB = OwnerComp->GetBlackboardComponent();
	BB->SetValueAsBool(ShouldBlockKey.SelectedKeyName, false);
	BB->SetValueAsBool(IsPlayerAttackingKey.SelectedKeyName, false);

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
