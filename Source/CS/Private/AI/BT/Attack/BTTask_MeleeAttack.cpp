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

					Combat->Execute_FirstAttack(NPC);

					if (UAnimInstance* AnimInst = NPC->GetMesh()->GetAnimInstance())
					{
						AnimInst->OnMontageEnded.AddDynamic(this, &UBTTask_MeleeAttack::OnMontageEnded);
					}

					return EBTNodeResult::InProgress;;
				}
			}
		}
	}
	
	return EBTNodeResult::Failed;
}

void UBTTask_MeleeAttack::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!CachedOwnerComp.IsValid()) return;

	if (UBlackboardComponent* BB = CachedOwnerComp.Get()->GetBlackboardComponent())
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
	}

	FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
}


bool UBTTask_MeleeAttack::MontageHasfinished(AAIBaseCharacter* const AI)
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
