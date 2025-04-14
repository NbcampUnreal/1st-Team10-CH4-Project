#include "AI/BT/BTTask_FindPlayerLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CSPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FindPlayerLocation::UBTTask_FindPlayerLocation(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = "Find Player Location In NavMash";
};

EBTNodeResult::Type UBTTask_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[BTTask] No controlled pawn found."));
		return EBTNodeResult::Failed;
	}

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

	if (!ClosestPlayer)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}

	const FVector PlayerLocation = ClosestPlayer->GetActorLocation();
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), ClosestPlayer);
	
	if (SearchRandom)
	{
		FNavLocation Loc;
		if (UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
		{
			if (NavSys->GetRandomPointInNavigableRadius(PlayerLocation, SearchRadius, Loc))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerLocation);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	
	return EBTNodeResult::Failed;
}