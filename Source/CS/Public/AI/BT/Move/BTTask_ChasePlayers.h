
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChasePlayers.generated.h"


UCLASS()
class CS_API UBTTask_ChasePlayers : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_ChasePlayers(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
