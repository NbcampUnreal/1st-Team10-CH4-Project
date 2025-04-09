#include "AI/BTService/BTService_DecideAttackType.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_DecideAttackType::UBTService_DecideAttackType()
{
	NodeName = TEXT("Decide Attack Type");
	
	AttackTypeKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DecideAttackType, AttackTypeKey));
}

void UBTService_DecideAttackType::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const int32 Rand = FMath::RandRange(0, 99);
	int32 ChosenAttack = 0;

	if (Rand < 40)
	{
		ChosenAttack = 1; 
	}
	else if (Rand < 90)
	{
		ChosenAttack = 0; 
	}
	else
	{
		ChosenAttack = 2; 
	}

	if (OwnerComp.GetBlackboardComponent())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(AttackTypeKey.SelectedKeyName, ChosenAttack);
	}
}

