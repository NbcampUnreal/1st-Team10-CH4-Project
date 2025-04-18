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

	if (BB->GetValueAsBool(FName("IsBusy")))
	{
		return EBTNodeResult::Failed;
	}

	if (auto* const Cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
	{
		APawn* AIPawn = Cont->GetPawn();
		AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));

		if (!AIPawn || !TargetActor) return EBTNodeResult::Failed;

		FVector MyLocation = AIPawn->GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();
		
		float Direction = FMath::Sign(TargetLocation.Y - MyLocation.Y);
		
		
		FVector DesiredLocation = TargetLocation - FVector(0.f, Direction * StopDistance, 0.f);
		DesiredLocation.X = MyLocation.X;
		DesiredLocation.Z = MyLocation.Z; 
		
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cont, DesiredLocation);
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

