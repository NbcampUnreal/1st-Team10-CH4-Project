// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Move/BTTask_Jump.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "GameFramework/Character.h"

UBTTask_Jump::UBTTask_Jump()
{
	NodeName = TEXT("Jump");
}

EBTNodeResult::Type UBTTask_Jump::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn());

	if (AIPawn)
	{
		AIPawn->Jump();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}