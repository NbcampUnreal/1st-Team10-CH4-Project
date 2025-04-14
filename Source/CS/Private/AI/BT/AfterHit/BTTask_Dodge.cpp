// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/AfterHit/BTTask_Dodge.h"

#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


UBTTask_Dodge::UBTTask_Dodge()
{
	NodeName = TEXT("Dodge");
	ShouldDodgeKey.SelectedKeyName = "ShouldDodge";
}

EBTNodeResult::Type UBTTask_Dodge::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	AAIBaseCharacter* NPC = Cast<AAIBaseCharacter>(AIController ? AIController->GetPawn() : nullptr);
	if (!NPC) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB || BB->GetValueAsBool(FName("IsHitReacting"))) return EBTNodeResult::Failed;

	AActor* Attacker = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Attacker) return EBTNodeResult::Failed;

	if (ICombatInterface* ICombat = Cast<ICombatInterface>(NPC))
	{
		BB->SetValueAsBool(FName("IsBusy"), true);

		int32 Result = ICombat->Execute_Dodge(NPC, Attacker);
		if (Result > 0)
		{
			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}
