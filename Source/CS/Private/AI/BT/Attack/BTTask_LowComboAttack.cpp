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
				
				NPC->StopMovement();
			
				BB->SetValueAsBool(FName("IsBusy"), true);
				CachedOwnerComp = &OwnerComp;

				Combat->Execute_LowComboAttack(NPC);

				if (UAnimInstance* AnimInst = NPC->GetMesh()->GetAnimInstance())
				{
					AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_LowComboAttack::OnMontageEnded);
				}

				return EBTNodeResult::InProgress;
				
			}
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_LowComboAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CachedOwnerComp.IsValid()) return;

	if (UBlackboardComponent* BB = CachedOwnerComp.Get()->GetBlackboardComponent())
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
		BB->SetValueAsInt(FName("AttackType"), 1);
	}

	FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
}


bool UBTTask_LowComboAttack::MontageHasfinished(AAIBaseCharacter* const AI)
{
	if (!AI || !AI->GetMesh() || !AI->GetLowComboAttackMontage()) return true;

	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	return AnimInstance && AnimInstance->Montage_GetIsStopped(AI->GetLowComboAttackMontage());
}
