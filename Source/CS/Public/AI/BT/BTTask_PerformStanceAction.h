// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_PerformStanceAction.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_PerformStanceAction : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_PerformStanceAction(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StanceKey;
};
