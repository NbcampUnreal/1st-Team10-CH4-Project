

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
	Count = 2;
}

EBTNodeResult::Type UBTTask_Block::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const bool bShouldBlock = OwnerComp.GetBlackboardComponent()->GetValueAsBool(ShouldBlockKey.SelectedKeyName);
	const bool bIsPlayerAttacking = OwnerComp.GetBlackboardComponent()->GetValueAsBool(IsPlayerAttackingKey.SelectedKeyName);
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	if (!(bShouldBlock || bIsPlayerAttacking))
	{
		return EBTNodeResult::Failed;
	}
	if (BB && BB->GetValueAsBool(FName("IsHitReacting")))
	{
		return EBTNodeResult::Failed;
	}
	
	auto* AIController = OwnerComp.GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;

	CachedCharacter = NPC;
	CachedOwnerComp = &OwnerComp;
	
	if (UAnimInstance* AnimInstance = NPC->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	if (auto* ICombat = Cast<ICombatInterface>(NPC))
	{
		if (ICombat->Execute_Block(NPC))
		{
			BB->SetValueAsBool(FName("IsBusy"), true);

			// 캐릭터 캐시
			CachedCharacter = NPC;

			// 델리게이트 바인딩
			if (UAnimInstance* AnimInst = NPC->GetMesh()->GetAnimInstance())
			{
				MontageEndDelegate.BindUObject(this, &UBTTask_Block::OnBlockMontageEnded);
				AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_Block::OnBlockMontageEnded);

			}

			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_Block::OnBlockMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CachedCharacter.IsValid() || !CachedOwnerComp.IsValid()) return;

	UBlackboardComponent* BB = CachedOwnerComp.Get()->GetBlackboardComponent();
	if (!BB) return;

	bool bPlayerStillAttacking = BB->GetValueAsBool(IsPlayerAttackingKey.SelectedKeyName);
	BlockCount = BB->GetValueAsInt(FName("BlockCount"));

	if (bPlayerStillAttacking)
	{
		BlockCount++;
		BB->SetValueAsInt(FName("BlockCount"), BlockCount);

		if (BlockCount >= Count)
		{
			BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), true);
		}
	}

	CachedCharacter->StopBlock();
	BB->SetValueAsBool(FName("ShouldBlock"), false);
	BB->SetValueAsBool(FName("IsBusy"), false);
	BB->SetValueAsInt(FName("BlockCount"), 0);

	FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
}
