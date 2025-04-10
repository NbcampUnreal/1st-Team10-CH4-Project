#include "AI/Controller/AIBossController.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIBossController::AAIBossController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{

}

void AAIBossController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Tree)
	{
		RunBehaviorTree(Tree);
	}
}