#include "AI/Controller/AINormalController.h"
#include "BehaviorTree/BlackboardComponent.h"

AAINormalController::AAINormalController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{

}

void AAINormalController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (Tree)
	{
		RunBehaviorTree(Tree);
	}
}