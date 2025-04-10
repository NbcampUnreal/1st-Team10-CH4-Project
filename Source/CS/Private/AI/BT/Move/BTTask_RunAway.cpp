#include "AI/BT/Move/BTTask_RunAway.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationPath.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_RunAway::UBTTask_RunAway()
{
	NodeName = TEXT("Run Away");
}

EBTNodeResult::Type UBTTask_RunAway::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		int32 Result = ICombat->Execute_RunAway(NPC, Attacker);

		if (Result > 0)
		{
			NPC->GetWorldTimerManager().SetTimer(
				RunAwayTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_RunAway::FinishRunAway, &OwnerComp),
				1.0f, false
			);
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_RunAway::FinishRunAway(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("ShouldRunAway"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
	}
	
	if (AAIController* AICon = OwnerComp->GetAIOwner())
	{
		if (AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon->GetPawn()))
		{
			AIPawn->ResumeMovement();
		}
	}
	
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

