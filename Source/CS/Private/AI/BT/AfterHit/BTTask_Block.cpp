

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

	auto* AIController = OwnerComp.GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;
	
	if (UAnimInstance* AnimInstance = NPC->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	if (auto* ICombat = Cast<ICombatInterface>(NPC))
	{
		BB->SetValueAsBool(FName("IsBusy"), true);

		if (ICombat->Execute_Block(NPC))
		{
			FTimerHandle BlockTimerHandle;
			NPC->GetWorldTimerManager().SetTimer(
				BlockTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_Block::FinishBlock, &OwnerComp),
				0.7f, false
			);

			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}


void UBTTask_Block::FinishBlock(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (!BB) return;
	auto* AIController = OwnerComp->GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());
	bool bPlayerStillAttacking = BB->GetValueAsBool(IsPlayerAttackingKey.SelectedKeyName);
	BlockCount = BB->GetValueAsInt(FName("BlockCount"));

	if (bPlayerStillAttacking)
	{
		BlockCount++;
		BB->SetValueAsInt(FName("BlockCount"), BlockCount);

		if (BlockCount >= Count)
		{
			NPC->StopBlock();
			BB->SetValueAsInt(FName("BlockCount"), 0);
			BB->SetValueAsBool(FName("ShouldBlock"), false);
			BB->SetValueAsBool(FName("IsBusy"), false);
			BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), true);
			FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			return;
		}

		if (FMath::FRand() < 0.7f)
		{
			FTimerHandle BlockTimerHandle;
			OwnerComp->GetWorld()->GetTimerManager().SetTimer(
				BlockTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_Block::FinishBlock, OwnerComp),
				0.6f, false
			);
			return;
		}
	}
	NPC->StopBlock();
	BB->SetValueAsBool(FName("ShouldBlock"), false);
	BB->SetValueAsBool(FName("IsBusy"), false);
	BB->SetValueAsInt(FName("BlockCount"), 0);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

