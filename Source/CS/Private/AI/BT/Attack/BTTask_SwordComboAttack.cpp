
#include "AI/BT/Attack/BTTask_SwordComboAttack.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_SwordComboAttack::UBTTask_SwordComboAttack(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Sword Combo Attack");
}

EBTNodeResult::Type UBTTask_SwordComboAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
				if (MontageHasfinished(NPC))
				{
					BB->SetValueAsBool(FName("IsBusy"), true);
					CachedOwnerComp = &OwnerComp;
					NPC->StopMovement();
					Combat->Execute_LowComboAttack(NPC);

					if (UAnimInstance* AnimInst = NPC->GetMesh()->GetAnimInstance())
					{
						AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_SwordComboAttack::OnMontageEnded);
					}

					return EBTNodeResult::InProgress;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_SwordComboAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CachedOwnerComp.IsValid()) return;

	UBehaviorTreeComponent* OwnerComp = CachedOwnerComp.Get();
	if (!OwnerComp) return;

	if (UBlackboardComponent* BB = OwnerComp->GetBlackboardComponent())
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
	}
	
	if (AAIController* Controller = OwnerComp->GetAIOwner())
	{
		if (AAIBaseCharacter* NPC = Cast<AAIBaseCharacter>(Controller->GetPawn()))
		{
			NPC->ResumeMovement();
		}
	}

	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

bool UBTTask_SwordComboAttack::MontageHasfinished(AAIBaseCharacter* const AI)
{
	if (!AI || !AI->GetMesh() || !AI->GetRangeComboAttackMontage()) return true;

	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	return AnimInstance && AnimInstance->Montage_GetIsStopped(AI->GetRangeComboAttackMontage());
}
