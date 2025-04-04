#include "AI/BT/BBTask_FindPathPoint.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBBTask_FindPathPoint::UBBTask_FindPathPoint(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Find Path Point");
}
EBTNodeResult::Type UBBTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* const cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const BC = OwnerComp.GetBlackboardComponent())
		{
			auto const Index = BC->GetValueAsInt((GetSelectedBlackboardKey()));
			if (auto* AI = Cast<AAIBaseCharacter>(cont->GetPawn()))
			{
				auto const Point = AI->GetPatrolPath()->GetPatrolPoint(Index);
				auto const GlobalPoint = AI->GetPatrolPath()->GetActorTransform().TransformPosition(Point);
				BC -> SetValueAsVector(PatrolPathVectorkey.SelectedKeyName, GlobalPoint);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}