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
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB || BB->GetValueAsBool(FName("IsHitReacting")))
	{
		return EBTNodeResult::Failed;
	}

	const auto* Controller = OwnerComp.GetAIOwner();
	auto* NPC = Controller ? Cast<AAIBaseCharacter>(Controller->GetPawn()) : nullptr;
	auto* Combat = NPC ? Cast<ICombatInterface>(NPC) : nullptr;
	FTimerHandle AttackCooldownTimerHandle;
	if (!Combat) return EBTNodeResult::Failed;
	
	BB->SetValueAsBool(FName("IsBusy"), true);
	
	Combat->Execute_FirstAttack(NPC);
	
	NPC->GetWorldTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		FTimerDelegate::CreateUObject(this, &UBTTask_MeleeNormalAttack::FinishLatentTaskEarly, &OwnerComp),
		0.8f, false
	);

	return EBTNodeResult::InProgress;
}



void UBTTask_MeleeNormalAttack::FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp)
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
				FTimerHandle AttackCooldownTimerHandle;
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
	if (!AI || !AI->GetMesh())
	{
		return true;
	}
	if (!AI->GetFirstAttackMontage())
	{
		return true;
	}
	auto* AnimInstance = AI->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return true;
	}
	const bool bStopped = AnimInstance->Montage_GetIsStopped(AI->GetFirstAttackMontage());
	return bStopped;
}