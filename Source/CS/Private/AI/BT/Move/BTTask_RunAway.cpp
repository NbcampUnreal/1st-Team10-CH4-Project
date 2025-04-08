// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Move/BTTask_RunAway.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"

UBTTask_RunAway::UBTTask_RunAway()
{
	NodeName = TEXT("RunAway");
}

EBTNodeResult::Type UBTTask_RunAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	APawn* AIPawn = AICon ? AICon->GetPawn() : nullptr;

	if (!AIPawn) return EBTNodeResult::Failed;

	FVector CurrentLocation = AIPawn->GetActorLocation();
	FVector RunDirection = -AIPawn->GetActorForwardVector();
	FVector RawTargetLocation = CurrentLocation + RunDirection * RunDistance;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation ValidTargetLocation;

	if (NavSys && NavSys->GetRandomPointInNavigableRadius(RawTargetLocation, 300.f, ValidTargetLocation))
	{
		AICon->MoveToLocation(ValidTargetLocation.Location);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
