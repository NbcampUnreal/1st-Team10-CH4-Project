// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/BTTask_ChooseRandomStance.h"

#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ChooseRandomStance::UBTTask_ChooseRandomStance(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Choose Random Stance");
}
EBTNodeResult::Type UBTTask_ChooseRandomStance::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	int32 RandomValue = FMath::RandRange(0, 2); // 0: Standing, 1: Jumping, 2: Crouching

	ECharacterStance Stance = static_cast<ECharacterStance>(RandomValue);
	OwnerComp.GetBlackboardComponent()->SetValueAsEnum(StanceKey.SelectedKeyName, static_cast<uint8>(Stance));

	return EBTNodeResult::Succeeded;
}

