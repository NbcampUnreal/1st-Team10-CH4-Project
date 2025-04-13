#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MeleeAttack.generated.h"

UCLASS()
class CS_API UBTTask_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	explicit UBTTask_MeleeAttack(FObjectInitializer const& ObjectInitializer);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:


	void FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp);
	bool MontageHasfinished(class AAIBaseCharacter* const AI);
};