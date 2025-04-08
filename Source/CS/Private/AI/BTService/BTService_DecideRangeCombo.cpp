#include "AI/BTService/BTService_DecideRangeCombo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

UBTService_DecideRangeCombo::UBTService_DecideRangeCombo()
{
	NodeName = TEXT("Decide Range Combo");
	bNotifyTick = true;

	RangeComboKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DecideRangeCombo, RangeComboKey));
}

void UBTService_DecideRangeCombo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto* Controller = OwnerComp.GetAIOwner();
	const auto* AIChar = Cast<AAIBaseCharacter>(Controller ? Controller->GetPawn() : nullptr);
	const auto* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (!AIChar || !Player) return;

	const float Distance = FVector::Dist(AIChar->GetActorLocation(), Player->GetActorLocation());

	if (Distance <= ComboTriggerDistance)
	{
		const int32 Rand = FMath::RandRange(0, 99);
		const bool bDoCombo = Rand < ComboChance;

		if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
		{
			BBComp->SetValueAsBool(RangeComboKey.SelectedKeyName, bDoCombo);
		}
	}
	else
	{
		if (UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent())
		{
			BBComp->SetValueAsBool(RangeComboKey.SelectedKeyName, false);
		}
	}
}
