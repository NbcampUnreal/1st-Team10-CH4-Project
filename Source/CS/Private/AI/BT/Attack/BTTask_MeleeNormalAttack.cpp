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
	
	if (BB && Combat)
	{

		BB->SetValueAsBool(FName("IsBusy"), true);
		CachedOwnerComp = &OwnerComp;

		Combat->Execute_FirstAttack(NPC);

		FTimerHandle MeleeTimerHandle;
		NPC->GetWorldTimerManager().SetTimer(
				MeleeTimerHandle,
				FTimerDelegate::CreateUObject(this, &UBTTask_MeleeNormalAttack::FinishAttack),
				1.0f, false
			);

		return EBTNodeResult::InProgress;;
	}
	return EBTNodeResult::Failed;
}


void UBTTask_MeleeNormalAttack::FinishAttack()
{
	if (!CachedOwnerComp.IsValid()) return;

	if (UBlackboardComponent* BB = CachedOwnerComp.Get()->GetBlackboardComponent())
	{
		BB->SetValueAsBool(FName("IsBusy"), false);
		BB->SetValueAsBool(FName("PlayerIsInMeleeRange"), false);
	}
	FinishLatentTask(*CachedOwnerComp.Get(), EBTNodeResult::Succeeded);
}

