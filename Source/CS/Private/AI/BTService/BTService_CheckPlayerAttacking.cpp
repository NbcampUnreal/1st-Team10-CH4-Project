#include "AI/BTService/BTService_CheckPlayerAttacking.h"

#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSPlayerCharacter.h"
#include "Components/CSCombatComponent.h"

UBTService_CheckPlayerAttacking::UBTService_CheckPlayerAttacking()
{
	NodeName = TEXT("Check Player Is Attacking");
	bNotifyTick = true;
}

void UBTService_CheckPlayerAttacking::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	if (BB->GetValueAsBool(FName("IsBusy"))) return;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));
	if (!Target) return;

	UCSCombatComponent* Combat = Target->FindComponentByClass<UCSCombatComponent>();
	if (!Combat) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AIController->GetPawn());
	if (!AIPawn) return;
	
	const float Distance = FVector::Dist(AIPawn->GetActorLocation(), Target->GetActorLocation());
	const float BlockReactionDistance = 200.f;

	if (Combat->GetIsAttacking() && Distance <= BlockReactionDistance)
	{
		if (FMath::FRand() < 0.5f)
		{
			BB->SetValueAsBool(FName("ShouldBlock"), true);
			BB->SetValueAsBool(FName("ShouldDodge"), false);
		}
	}
}
