// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BT/Attack/BTTask_LowComboAttack.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_LowComboAttack::UBTTask_LowComboAttack(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Low Combo Attack");
}

EBTNodeResult::Type UBTTask_LowComboAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const bool bOutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	if (bOutOfRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	

	if (BB && BB->GetValueAsBool(FName("IsHitReacting")))
	{
		return EBTNodeResult::Failed;
	}
	
	if (const auto* Controller = OwnerComp.GetAIOwner())
	{
		if (auto* NPC = Cast<AAIBaseCharacter>(Controller->GetPawn()))
		{
			if (auto* Combat = Cast<ICombatInterface>(NPC))
			{
				BB->SetValueAsBool(FName("IsBusy"), true);
				;
				NPC->StopMovement();
			
				if (MontageHasfinished(NPC))
				{
					Combat->Execute_LowComboAttack(NPC);
					FTimerHandle AttackCooldownTimerHandle;
					NPC->GetWorldTimerManager().SetTimer(
						AttackCooldownTimerHandle,
						FTimerDelegate::CreateUObject(this, &UBTTask_LowComboAttack::FinishLatentTaskEarly, &OwnerComp),
						2.0f, false
					);
					return EBTNodeResult::InProgress;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}


void UBTTask_LowComboAttack::FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	if (auto* Controller = OwnerComp->GetAIOwner())
	{
		if (auto* NPC = Cast<AAIBaseCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* BBComp = OwnerComp->GetBlackboardComponent())
			{
				BBComp->SetValueAsBool(FName("IsBusy"), false);
				BBComp->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
				BBComp->SetValueAsInt(FName("AttackType"), 1);
			}

			if (MontageHasfinished(NPC))
			{
				NPC->ResumeMovement();
				FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				FTimerHandle AttackCooldownTimerHandle;
				NPC->GetWorldTimerManager().SetTimer(
					AttackCooldownTimerHandle,
					FTimerDelegate::CreateUObject(this, &UBTTask_LowComboAttack::FinishLatentTaskEarly, OwnerComp),
					0.2f, false
				);
			}
		}
	}
}


bool UBTTask_LowComboAttack::MontageHasfinished(AAIBaseCharacter* const AI)
{
	if (!AI || !AI->GetMesh() || !AI->GetLowComboAttackMontage()) return true;

	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	return AnimInstance && AnimInstance->Montage_GetIsStopped(AI->GetLowComboAttackMontage());
}
