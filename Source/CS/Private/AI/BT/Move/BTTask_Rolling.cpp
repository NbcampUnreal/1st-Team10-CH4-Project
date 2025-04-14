// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Move/BTTask_Rolling.h"

#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Rolling::UBTTask_Rolling()
{
	NodeName = TEXT("Rolling");
}

EBTNodeResult::Type UBTTask_Rolling::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AAIBaseCharacter* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	
	AActor* Attacker = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Attacker) return EBTNodeResult::Failed;

	if (ICombatInterface* ICombat = Cast<ICombatInterface>(NPC))
	{
		BB->SetValueAsBool(FName("IsBusy"), true);
		int32 Result = ICombat->Execute_Rolling(NPC);

		if (Result > 0)
		{
			FTimerHandle RunAwayTimerHandle;
			NPC->GetWorldTimerManager().SetTimer(
				RunAwayTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_Rolling::FinishRunAway, &OwnerComp),
				0.8f, false
			);
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_Rolling::FinishRunAway(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("ShouldRolling"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
	}
	
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
