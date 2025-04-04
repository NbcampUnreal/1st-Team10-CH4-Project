#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MeleeAttack.generated.h"


UCLASS()
class CS_API UBTTask_MeleeAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_MeleeAttack();
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void ResetCanAttack(UBehaviorTreeComponent* OwnerComp);
	
private:
	bool MontageHasfinished(AAIBaseCharacter* const AI);
	
	FTimerHandle AttackCooldownTimerHandle;
};
