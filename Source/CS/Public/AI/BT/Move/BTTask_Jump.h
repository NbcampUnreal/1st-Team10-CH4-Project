// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Jump.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_Jump : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_Jump();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void FinishJump(UBehaviorTreeComponent* OwnerComp);
};