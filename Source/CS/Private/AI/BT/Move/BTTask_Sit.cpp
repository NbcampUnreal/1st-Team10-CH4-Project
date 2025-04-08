// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Move/BTTask_Sit.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "GameFramework/Character.h"

UBTTask_Sit::UBTTask_Sit()
{
	NodeName = TEXT("Sit");
}

EBTNodeResult::Type UBTTask_Sit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn());

	if (AIPawn)
	{
		UAnimMontage* SitMontage = AIPawn->GetSitMontage();
		AIPawn->PlayAnimMontage(SitMontage); 
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}