#include "AI/BT/Attack/BTTask_CounterAttack.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CounterAttack::UBTTask_CounterAttack()
{
	NodeName = TEXT("Counter Attack");
}

EBTNodeResult::Type UBTTask_CounterAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto* AIController = OwnerComp.GetAIOwner();
	auto* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());

	if (auto* ICombat = Cast<ICombatInterface>(NPC))
	{
		NPC->GetWorldTimerManager().ClearTimer(CounterTimerHandle);
		ICombat->Execute_firstAttack(NPC);

		NPC->GetWorldTimerManager().SetTimer(
			CounterTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_CounterAttack::EndCounter, &OwnerComp),
			8.0f, false // 반격 후 쿨타임
		);

		return EBTNodeResult::InProgress;
	}

	return EBTNodeResult::Failed;
}

void UBTTask_CounterAttack::EndCounter(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerComp)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
}
