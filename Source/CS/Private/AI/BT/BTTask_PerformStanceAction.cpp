// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/BTTask_PerformStanceAction.h"

#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Character/AIBossCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PerformStanceAction::UBTTask_PerformStanceAction(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Perform Stance Action");
}
EBTNodeResult::Type UBTTask_PerformStanceAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AIBoss = Cast<AAIBossCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!AIBoss) return EBTNodeResult::Failed;

	ECharacterStance Stance = static_cast<ECharacterStance>(OwnerComp.GetBlackboardComponent()->GetValueAsEnum(StanceKey.SelectedKeyName));

	switch (Stance)
	{
	case ECharacterStance::Jumping:
		AIBoss->JumpAction();
		break;
	case ECharacterStance::Crouching:
		AIBoss->CrouchAction();
		break;
	default:
		break; 
	}

	return EBTNodeResult::Succeeded;
}
