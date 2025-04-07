// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_CounterAttack.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_CounterAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_CounterAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FTimerHandle CounterTimerHandle;
	void EndCounter(UBehaviorTreeComponent* OwnerComp);
};
