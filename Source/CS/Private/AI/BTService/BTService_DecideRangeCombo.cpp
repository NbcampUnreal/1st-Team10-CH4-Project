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

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AICon = OwnerComp.GetAIOwner();
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(AICon ? AICon->GetPawn() : nullptr);
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(FName("TargetActor")));

	if (!AIPawn || !Target || !BB) return;
	
	if (BB->GetValueAsBool(FName("RangeCombo")))
	{
		return;
	}

	const float Distance = FVector::Dist(AIPawn->GetActorLocation(), Target->GetActorLocation());

	if (Distance >= MinRange && Distance <= MaxRange && FMath::FRand() < Chance)
	{
		BB->SetValueAsBool(FName("RangeCombo"), true);
	}
	else
	{
		BB->SetValueAsBool(FName("RangeCombo"), false);
	}
}

