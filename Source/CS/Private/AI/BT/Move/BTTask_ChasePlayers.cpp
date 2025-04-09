#include "AI/BT/Move/BTTask_ChasePlayers.h"

#include "AI/Controller/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_ChasePlayers::UBTTask_ChasePlayers(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = "ChasePlayers";
};
EBTNodeResult::Type UBTTask_ChasePlayers::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;
	bool bIsBusy = BB->GetValueAsBool(FName("IsBusy"));
	if (bIsBusy)
	{
		return EBTNodeResult::Failed;
	}

	if (auto* const Cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
	{
		FVector PlayerLocation = BB->GetValueAsVector(GetSelectedBlackboardKey());
		
		BB->SetValueAsBool(FName("IsBusy"), true);
		
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cont, PlayerLocation);
		
		BB->SetValueAsBool(FName("IsBusy"), false);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
