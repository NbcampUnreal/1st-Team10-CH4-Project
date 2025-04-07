// UBTTask_Block.cpp

#include "AI/BT/BTTask_Block.h"
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

	//  둘 다 false일 때만 실패
	if (!(bShouldBlock || bIsPlayerAttacking))
	{
		UE_LOG(LogTemp, Warning, TEXT(" BlockTask 실패: 조건 불충분 (ShouldBlock: %d, IsPlayerAttacking: %d)"), bShouldBlock, bIsPlayerAttacking);
		return EBTNodeResult::Failed;
	}

	auto* AIController = OwnerComp.GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());

	if (auto* ICombat = Cast<ICombatInterface>(NPC))
	{
		UE_LOG(LogTemp, Warning, TEXT("🛡 방어 시작"));

		if (ICombat->Execute_Block(NPC))
		{
			// 타이머로 방어 유지 후 반격 시도
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
		UE_LOG(LogTemp, Warning, TEXT(" 방어 종료 → ⚔ 반격 시작"));
		ICombat->Execute_MeleeAttack(NPC);  // 반격
	}

	// 블랙보드 초기화
	auto* BB = OwnerComp->GetBlackboardComponent();
	BB->SetValueAsBool(ShouldBlockKey.SelectedKeyName, false);
	BB->SetValueAsBool(IsPlayerAttackingKey.SelectedKeyName, false);

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
