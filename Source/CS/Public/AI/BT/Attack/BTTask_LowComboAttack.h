// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_LowComboAttack.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_LowComboAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_LowComboAttack(FObjectInitializer const& ObjectInitializer);
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:

	void FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp);
	bool MontageHasfinished(class AAIBaseCharacter* const AI);
};
