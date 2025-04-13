// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RunAway.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_RunAway : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTTask_RunAway();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void FinishRunAway(UBehaviorTreeComponent* OwnerComp);

	UPROPERTY(EditAnywhere, Category = "RunAway")
	float RunDistance = 200.f;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};