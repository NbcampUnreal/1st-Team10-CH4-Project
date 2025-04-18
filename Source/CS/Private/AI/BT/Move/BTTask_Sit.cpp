
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

	if (UAnimInstance* AnimInstance = AIPawn->GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	UAnimMontage* SitMontage = AIPawn->GetSitMontage();
	if (SitMontage)
	{
		BB->SetValueAsBool(FName("IsBusy"), true);
		AIPawn->StopMovement();

		AIPawn->PlayAnimMontage(SitMontage);
		FTimerHandle SitFinishHandle;
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
	if (!OwnerComp) return;
	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (AAIController* AICon = OwnerComp->GetAIOwner())
	{
		if (AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn()))
		{
			BB->SetValueAsBool(FName("ShouldCrouch"), false);
			BB->SetValueAsBool(FName("IsBusy"), false);
			if (AIPawn->IsValidLowLevel())
			{
				AIPawn->ResumeMovement();
			}
		}
	}
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
