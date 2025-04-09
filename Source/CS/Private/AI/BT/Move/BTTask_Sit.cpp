// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Move/BTTask_Sit.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTTask_Sit::UBTTask_Sit()
{
	NodeName = TEXT("Sit");
}

EBTNodeResult::Type UBTTask_Sit::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn());

	if (!AIPawn) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	
	UAnimMontage* SitMontage = AIPawn->GetSitMontage();
	if (SitMontage)
	{
		BB->SetValueAsBool(FName("IsBusy"), true);
		AIPawn->StopMovement();
		
		AIPawn->GetWorldTimerManager().SetTimer(
			SitFinishHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_Sit::FinishSit, &OwnerComp),
			0.8f, false
		);
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;
}

void UBTTask_Sit::FinishSit(UBehaviorTreeComponent* OwnerComp)
{
	AAIController* AICon = OwnerComp->GetAIOwner();
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn());
	AIPawn->ResumeMovement();
	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("ShouldCrouch"), false);
	}
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
