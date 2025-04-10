#include "AI/Controller/AIFighterController.h"


AAIFighterController::AAIFighterController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
}

void AAIFighterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Tree)
	{
		RunBehaviorTree(Tree);
	}
}