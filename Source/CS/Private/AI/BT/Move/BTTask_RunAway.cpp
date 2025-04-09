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
	BB->SetValueAsBool(FName("IsBusy"), true);
	AActor* Attacker = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Attacker) return EBTNodeResult::Failed;

	if (ICombatInterface* ICombat = Cast<ICombatInterface>(NPC))
	{
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
	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("ShouldRunAway"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
		AAIController* AICon = OwnerComp->GetAIOwner();
		AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon ? AICon->GetPawn() : nullptr);
		AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));

		if (AIPawn && TargetActor)
		{
			FVector MyLoc = AIPawn->GetActorLocation();
			FVector TargetLoc = TargetActor->GetActorLocation();
			float DirectionY = MyLoc.Y < TargetLoc.Y ? 1.f : -1.f;
			FRotator NewRot = FRotator(0.f, DirectionY > 0.f ? 90.f : -90.f, 0.f);
			AIPawn->SetActorRotation(NewRot);
			AICon->SetControlRotation(NewRot);
			
		}
	}
	
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

