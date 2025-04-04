
#include "AI/BTService/BTService_IsPlayerInMeleeRange.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_IsPlayerInMeleeRange::UBTService_IsPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Is Player In Melee Range";
}

void UBTService_IsPlayerInMeleeRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const * const Cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner());
	auto const * const AI = Cast<AAIBaseCharacter>(Cont->GetPawn());

	auto const * const player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	OwnerComp.GetBlackboardComponent()->SetValueAsBool(
		GetSelectedBlackboardKey(),
		AI->GetDistanceTo(player) <= MeleeRange); 
}
