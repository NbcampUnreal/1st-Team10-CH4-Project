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
	AAIBaseCharacter* NPC = Cast<AAIBaseCharacter>(AIController->GetPawn());
	if (!NPC) return EBTNodeResult::Failed;
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	
	AActor* Attacker = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Attacker) return EBTNodeResult::Failed;

	if (ICombatInterface* ICombat = Cast<ICombatInterface>(NPC))
	{
		BB->SetValueAsBool(FName("IsBusy"), true);
		int32 Result = ICombat->Execute_Dodge(NPC, Attacker);

		if (Result > 0)
		{
			
			NPC->GetWorldTimerManager().SetTimer(
				DodgeTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_Dodge::FinishDodge, &OwnerComp),
				1.0f, false
			);

			return EBTNodeResult::InProgress;
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_Dodge::FinishDodge(UBehaviorTreeComponent* OwnerComp)
{
	UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
		BB->SetValueAsBool(ShouldDodgeKey.SelectedKeyName, false);
		BB->SetValueAsBool(FName("IsBusy"), false);
		
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