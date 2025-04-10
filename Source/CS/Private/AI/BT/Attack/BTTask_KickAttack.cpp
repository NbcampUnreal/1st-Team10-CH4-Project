
#include "AI/BT/Attack/BTTask_KickAttack.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

UBTTask_KickAttack::UBTTask_KickAttack(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Kick Attack");
}

EBTNodeResult::Type UBTTask_KickAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

					Combat->Execute_secondAttack(NPC);
					
					NPC->GetWorldTimerManager().SetTimer(
						AttackCooldownTimerHandle,
						FTimerDelegate::CreateUObject(this, &UBTTask_KickAttack::FinishLatentTaskEarly, &OwnerComp),
						0.4f, false
					);

					return EBTNodeResult::InProgress;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}

void UBTTask_KickAttack::FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp)
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
			}
			if (MontageHasfinished(NPC))
			{
				FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				
				NPC->GetWorldTimerManager().SetTimer(
					AttackCooldownTimerHandle,
					FTimerDelegate::CreateUObject(this, &UBTTask_KickAttack::FinishLatentTaskEarly, OwnerComp),
					0.2f, false
				);
			}
		}
	}
}

bool UBTTask_KickAttack::MontageHasfinished(AAIBaseCharacter* const AI)
{
	if (!AI || !AI->GetMesh() || !AI->GetsecondAttackMontage()) return true;

	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	return AnimInstance && AnimInstance->Montage_GetIsStopped(AI->GetsecondAttackMontage());
}
