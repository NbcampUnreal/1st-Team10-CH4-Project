
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

void UBTService_IsPlayerInMeleeRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;
	bool bIsBusy = BB->GetValueAsBool(FName("IsBusy"));
	if (bIsBusy)
	{
		return;
	}
	if (auto* Cont = Cast<AAIBaseController>(OwnerComp.GetAIOwner()))
	{
		if (auto* AI = Cast<AAIBaseCharacter>(Cont->GetPawn()))
		{
			auto* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			if (Player)
			{
				bool bInRange = AI->GetDistanceTo(Player) <= MeleeRange;
				OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), bInRange);
			}
		}
	}
}