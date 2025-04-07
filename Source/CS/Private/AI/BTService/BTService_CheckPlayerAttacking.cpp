#include "AI/BTService/BTService_CheckPlayerAttacking.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSPlayerCharacter.h"
#include "Components/CSCombatComponent.h"

UBTService_CheckPlayerAttacking::UBTService_CheckPlayerAttacking()
{
	NodeName = TEXT("Check Player Is Attacking");
}

void UBTService_CheckPlayerAttacking::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ACSPlayerCharacter* Player = Cast<ACSPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!Player) return;

	UCSCombatComponent* CombatComp = Player->FindComponentByClass<UCSCombatComponent>();
	if (!CombatComp) return;

	bool bIsAttacking = CombatComp->GetIsAttacking();
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(GetSelectedBlackboardKey(), bIsAttacking);
}
