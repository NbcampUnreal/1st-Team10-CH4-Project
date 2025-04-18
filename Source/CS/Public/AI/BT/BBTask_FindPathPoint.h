
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BBTask_FindPathPoint.generated.h"


UCLASS()
class CS_API UBBTask_FindPathPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBBTask_FindPathPoint(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere, Category = "Blackborad",meta= (AllowPrivateAccess=true))
	FBlackboardKeySelector PatrolPathVectorkey;
};
