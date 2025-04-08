#include "AI/BT/Attack/BTTask_MeleeAttack.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Interface/CombatInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/GameplayStatics.h"

UBTTask_MeleeAttack::UBTTask_MeleeAttack(FObjectInitializer const& ObjectInitializer) :
UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Melee Attack");
}

EBTNodeResult::Type UBTTask_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const bool bOutOfRange = !OwnerComp.GetBlackboardComponent()->GetValueAsBool(GetSelectedBlackboardKey());
	

	if (bOutOfRange)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
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
					
					Combat->Execute_MeleeAttack(NPC);

					NPC->GetWorldTimerManager().SetTimer(
						AttackCooldownTimerHandle,
						FTimerDelegate::CreateUObject(this, &UBTTask_MeleeAttack::FinishLatentTaskEarly, &OwnerComp),
						0.3f, false
					);

					return EBTNodeResult::InProgress;
				}
			
			}
		
		}
	
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_MeleeAttack::FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp)
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
					FTimerDelegate::CreateUObject(this, &UBTTask_MeleeAttack::FinishLatentTaskEarly, OwnerComp),
					0.2f, false
				);
			}
		}
	}
}

bool UBTTask_MeleeAttack::MontageHasfinished(AAIBaseCharacter* const AI)
{
	if (!AI || !AI->GetMesh())
	{
		return true;
	}
	if (!AI->GetPunchMontage())
	{
		return true;
	}
	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return true;
	}
	const bool bStopped = AnimInstance->Montage_GetIsStopped(AI->GetPunchMontage());
	return bStopped;
}
