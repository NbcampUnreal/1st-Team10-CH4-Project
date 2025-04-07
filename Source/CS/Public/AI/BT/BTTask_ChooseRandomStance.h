// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_ChooseRandomStance.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_ChooseRandomStance : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_ChooseRandomStance(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StanceKey;
};
