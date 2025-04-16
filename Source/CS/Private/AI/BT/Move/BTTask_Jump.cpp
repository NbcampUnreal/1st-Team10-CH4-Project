// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Move/BTTask_Jump.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_Jump::UBTTask_Jump()
{
	NodeName = TEXT("Jump");
}

EBTNodeResult::Type UBTTask_Jump::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn());

	if (!AIPawn) return EBTNodeResult::Failed;
	
	if (UAnimInstance* AnimInstance = AIPawn->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (BB && BB->GetValueAsBool(FName("IsHitReacting")))
	{
		return EBTNodeResult::Failed;
	}
	BB->SetValueAsBool(FName("IsBusy"), true);

	AIPawn->Jump();
	FTimerHandle JumpFinishHandle;
	AIPawn->GetWorldTimerManager().SetTimer(
		JumpFinishHandle,
		FTimerDelegate::CreateUObject(this, &UBTTask_Jump::FinishJump, &OwnerComp),
		1.0f, false
	);

	return EBTNodeResult::InProgress;
}


void UBTTask_Jump::FinishJump(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("ShouldJump"), false);
	}

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
