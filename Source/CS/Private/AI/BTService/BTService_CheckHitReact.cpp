#include "AI/BTService//BTService_CheckHitReact.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_CheckHitReact::UBTService_CheckHitReact()
{
	NodeName = TEXT("Check HitReact -> Should Block");
	Interval = 0.1f;
	RandomDeviation = 0.f;
	ShouldBlockKey.SelectedKeyName = "ShouldBlock";
}

void UBTService_CheckHitReact::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	AAIBaseCharacter* AICharacter = Cast<AAIBaseCharacter>(AIController->GetPawn());

	if (AICharacter == nullptr) return;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();

	// 이미 BlockTask 중이면 감지하지 않도록 예외 처리
	bool bIsBlockingNow = BB->GetValueAsBool(ShouldBlockKey.SelectedKeyName);
	if (bIsBlockingNow) return;

	// HitReact 중일 때만 true로 전환
	bool bShouldBlock = AICharacter->IsInHitReact();
	if (bShouldBlock)
	{
		BB->SetValueAsBool(ShouldBlockKey.SelectedKeyName, true);
		UE_LOG(LogTemp, Warning, TEXT("BTService: Set ShouldBlock = true"));
	}
}
