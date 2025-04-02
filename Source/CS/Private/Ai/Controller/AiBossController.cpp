#include "Ai/Controller/AIBossController.h"

AAIBossController::AAIBossController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAsset(TEXT("/Game/AI/BT_BossAI"));
	if (BTAsset.Succeeded())
	{
		BehaviorTreeAsset = BTAsset.Object;
	}
}
