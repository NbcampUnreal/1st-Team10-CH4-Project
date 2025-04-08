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

	if (Rand < 10)
	{
		ChosenAttack = 1; 
	}
	else if (Rand < 50)
	{
		ChosenAttack = 0; 
	}
	else
	{
		ChosenAttack = 2; 
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTService_DecideAttackType] Rand: %d -> Chosen: %d"), Rand, ChosenAttack);

	if (OwnerComp.GetBlackboardComponent())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(AttackTypeKey.SelectedKeyName, ChosenAttack);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[BTService_DecideAttackType] BlackboardComponent is null!"));
	}
}

