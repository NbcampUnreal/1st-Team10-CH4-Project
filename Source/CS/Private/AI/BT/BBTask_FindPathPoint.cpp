#include "AI/BT/BBTask_FindPathPoint.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/PatrolPath.h"
#include "BehaviorTree/BlackboardComponent.h"

UBBTask_FindPathPoint::UBBTask_FindPathPoint(FObjectInitializer const& ObjectInitializer)
	: UBTTask_BlackboardBase{ ObjectInitializer }
{
	NodeName = TEXT("Find Path Point");
}

EBTNodeResult::Type UBBTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIBaseController* const Controller = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
	{
		if (UBlackboardComponent* const BB = OwnerComp.GetBlackboardComponent())
		{
			const int32 Index = BB->GetValueAsInt(GetSelectedBlackboardKey());

			if (AAIBaseCharacter* const AI = Cast<AAIBaseCharacter>(Controller->GetPawn()))
			{
				APatrolPath* PatrolPath = AI->GetPatrolPath();

				if (!PatrolPath)
				{
					return EBTNodeResult::Failed;
				}

				if (Index < 0 || Index >= PatrolPath->Num())
				{
					return EBTNodeResult::Failed;
				}

				FVector const Point = PatrolPath->GetPatrolPoint(Index);
				FVector const GlobalPoint = PatrolPath->GetActorTransform().TransformPosition(Point);
				BB->SetValueAsVector(PatrolPathVectorkey.SelectedKeyName, GlobalPoint);

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	
	return EBTNodeResult::Failed;
}
