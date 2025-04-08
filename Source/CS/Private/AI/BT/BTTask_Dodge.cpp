// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/BTTask_Dodge.h"

#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTTask_Dodge::UBTTask_Dodge()
{
	NodeName = TEXT("Dodge");
	ShouldDodgeKey.SelectedKeyName = "ShouldDodge";
}

EBTNodeResult::Type UBTTask_Dodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AAIBaseCharacter* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;


	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Attacker = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Attacker) return EBTNodeResult::Failed;

	if (ICombatInterface* ICombat = Cast<ICombatInterface>(NPC))
	{
		int32 Result = ICombat->Execute_Dodge(NPC, Attacker);

		if (Result > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("회피 행동 시작"));

			NPC->GetWorldTimerManager().SetTimer(
				DodgeTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_Dodge::FinishDodge, &OwnerComp),
				1.5f, false
			);

			return EBTNodeResult::InProgress;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("회피 행동 실패"));
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_Dodge::FinishDodge(UBehaviorTreeComponent* OwnerComp)
{
	UE_LOG(LogTemp, Log, TEXT("회피 종료 및 태스크 완료"));

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}