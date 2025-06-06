#include "AI/BTService/BTService_IsPlayerInMeleeRange.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_IsPlayerInMeleeRange::UBTService_IsPlayerInMeleeRange()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Is Player In Melee Range";
}

void UBTService_IsPlayerInMeleeRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (auto* BB = OwnerComp.GetBlackboardComponent())
	{
		if (auto* Cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
		{
			if (auto* AI = Cast<AAIBaseCharacter>(Cont->GetPawn()))
			{
				UObject* TargetObject = BB->GetValueAsObject("TargetActor");
				if (auto* TargetActor = Cast<AActor>(TargetObject))
				{
					const float Distance = AI->GetDistanceTo(TargetActor);
					const bool bInRange = Distance <= MeleeRange;

					BB->SetValueAsBool(GetSelectedBlackboardKey(), bInRange);
				}
			}
		}
	}
}
