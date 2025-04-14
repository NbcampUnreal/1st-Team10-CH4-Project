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
	
	if (MontageHasfinished(NPC))
	{

		BB->SetValueAsBool(FName("IsBusy"), true);
		CachedOwnerComp = &OwnerComp;

		Combat->Execute_SecondAttack(NPC);

		if (UAnimInstance* AnimInst = NPC->GetMesh()->GetAnimInstance())
		{
			AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_MeleeNormalAttack::OnMontageEnded);
		}

		return EBTNodeResult::InProgress;;
	}
	return EBTNodeResult::Failed;
}


void UBTTask_MeleeNormalAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CachedOwnerComp.IsValid()) return;

	if (UBlackboardComponent* BB = CachedOwnerComp.Get()->GetBlackboardComponent())
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
	}

	FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
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