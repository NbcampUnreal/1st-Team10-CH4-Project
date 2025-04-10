#include "AI/Controller/AISwordManController.h"


AAISwordManController::AAISwordManController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
}

void AAISwordManController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (Tree)
	{
		RunBehaviorTree(Tree);
	}
}