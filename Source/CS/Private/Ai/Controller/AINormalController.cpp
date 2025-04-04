#include "AI/Controller/AINormalController.h"
#include "BehaviorTree/BlackboardComponent.h"

AAINormalController::AAINormalController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
	/*static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/AI/BT_NormalAI"));
	if (BTAsset.Succeeded())
	{
		Tree = BTAsset.Object;
	}*/
}

void AAINormalController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Boss 전용 BehaviorTree 실행
	if (Tree)
	{
		RunBehaviorTree(Tree);
	}
}