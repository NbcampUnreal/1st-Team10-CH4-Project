
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CheckPlayerAttacking.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTService_CheckPlayerAttacking : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_CheckPlayerAttacking();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
