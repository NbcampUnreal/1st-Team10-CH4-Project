#include "AI/Controller/AIBossController.h"
#include "BehaviorTree/BlackboardComponent.h"

AAIBossController::AAIBossController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
	/*static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/AI/BT_BossAI"));
	if (BTAsset.Succeeded())
	{
		//Tree = BTAsset.Object;
	}*/
}

void AAIBossController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// Boss 전용 BehaviorTree 실행
	if (Tree)
	{
		RunBehaviorTree(Tree);
	}
}