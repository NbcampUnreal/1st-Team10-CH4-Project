#include "AI/BTService/BTService_FindClosestPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Characters/CSPlayerCharacter.h"
#include "GameFramework/Character.h"

UBTService_FindClosestPlayer::UBTService_FindClosestPlayer()
{
	NodeName = "Find Closest Player";
	Interval = 0.2f;
}

void UBTService_FindClosestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn) return;

	TArray<AActor*> Players;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACSPlayerCharacter::StaticClass(), Players);

	AActor* ClosestPlayer = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	for (AActor* Player : Players)
	{
		if (!Player || Player == ControlledPawn) continue;

		float Distance = FVector::Dist(Player->GetActorLocation(), ControlledPawn->GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestPlayer = Player;
		}
	}

	if (ClosestPlayer)
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		if (Blackboard)
		{
			Blackboard->SetValueAsObject(FName("TargetActor"), ClosestPlayer);
		}
	}
}
