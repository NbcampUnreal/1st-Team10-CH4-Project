#include "AI/BT/Attack/BTTask_MeleeNormalAttack.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_MeleeNormalAttack::UBTTask_MeleeNormalAttack(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UBTTask_MeleeNormalAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
				NPC->GetWorldTimerManager().ClearTimer(AttackCooldownTimerHandle);
				if (MontageHasfinished(NPC))
				{

					BB->SetValueAsBool(FName("IsBusy"), true);

					Combat->Execute_firstAttack(NPC);

					NPC->GetWorldTimerManager().SetTimer(
						AttackCooldownTimerHandle,
						FTimerDelegate::CreateUObject(this, &UBTTask_MeleeNormalAttack::FinishLatentTaskEarly, &OwnerComp),
						0.8f, false
					);

					return EBTNodeResult::InProgress;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}


void UBTTask_MeleeNormalAttack::FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp) return;

	if (auto* Controller = OwnerComp->GetAIOwner())
	{
		if (auto* NPC = Cast<AAIBaseCharacter>(Controller->GetPawn()))
		{
			
			if (MontageHasfinished(NPC))
			{
				FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				NPC->GetWorldTimerManager().SetTimer(
					AttackCooldownTimerHandle,
					FTimerDelegate::CreateUObject(this, &UBTTask_MeleeNormalAttack::FinishLatentTaskEarly, OwnerComp),
					0.2f, false
				);
			}
		}
	}
}

bool UBTTask_MeleeNormalAttack::MontageHasfinished(AAIBaseCharacter* const AI)
{
	if (!AI || !AI->GetMesh() || !AI->GetfirstAttackMontage()) return true;

	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	return AnimInstance && AnimInstance->Montage_GetIsStopped(AI->GetfirstAttackMontage());
}
