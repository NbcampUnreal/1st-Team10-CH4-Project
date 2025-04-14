
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Rolling.generated.h"

UCLASS()
class CS_API UBTTask_Rolling : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_Rolling();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void FinishRunAway(UBehaviorTreeComponent* OwnerComp);
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
