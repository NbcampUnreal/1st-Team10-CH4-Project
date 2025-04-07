// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Block.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_Block : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_Block();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	void FinishBlock(UBehaviorTreeComponent* OwnerComp);

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldBlockKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsPlayerAttackingKey;

	FTimerHandle BlockTimerHandle;
};
