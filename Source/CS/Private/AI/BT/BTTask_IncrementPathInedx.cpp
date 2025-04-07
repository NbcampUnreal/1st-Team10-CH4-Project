#include "AI/BT/BTTask_IncrementPathInedx.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_IncrementPathInedx::UBTTask_IncrementPathInedx(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Increment Path Index");
}
EBTNodeResult::Type UBTTask_IncrementPathInedx::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const Cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const AI = Cast<AAIBaseCharacter>(Cont->GetPawn()))
		{
			if (auto* const BC = OwnerComp.GetBlackboardComponent())
			{
				auto const NoOfPoints = AI->GetPatrolPath()->Num();
				auto const MinIndex = 0;
				auto const MaxIndex = NoOfPoints - 1;
				auto Index = BC->GetValueAsInt(GetSelectedBlackboardKey());

				if (bBiDirectional)
				{
					if (Index >= MaxIndex && Direction == EDirectionType::Forward)
					{
						Direction = EDirectionType::Reverse;
					}
					else if (Index >= MinIndex && Direction == EDirectionType::Reverse)
					{
						Direction = EDirectionType::Forward;
					}
				}

				Index = (Direction == EDirectionType::Forward) ? Index + 1 : Index - 1;
				Index = (Index + NoOfPoints) % NoOfPoints; 

				BC->SetValueAsInt(GetSelectedBlackboardKey(), Index);
				
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;

			}
		}
	}
	return EBTNodeResult::Failed;
}