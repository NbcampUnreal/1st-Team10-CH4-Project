#include "AI/BTService/BTService_CheckIsBusy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

const FName UBTService_CheckIsBusy::IsBusyKeyName(TEXT("IsBusy"));

UBTService_CheckIsBusy::UBTService_CheckIsBusy()
{
	NodeName = TEXT("Check IsBusy");
	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UBTService_CheckIsBusy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	FBTIsBusyMemory* BusyMemory = reinterpret_cast<FBTIsBusyMemory*>(NodeMemory);
	if (!BusyMemory) return;

	const bool bIsBusy = BB->GetValueAsBool(IsBusyKeyName);

	if (bIsBusy)
	{
		BusyMemory->ElapsedBusyTime += DeltaSeconds;

		if (BusyMemory->ElapsedBusyTime > 1.5f)
		{
			BB->SetValueAsBool(IsBusyKeyName, false);
			BusyMemory->ElapsedBusyTime = 0.f;
		}
	}
	else
	{
		BusyMemory->ElapsedBusyTime = 0.f;
	}
}
