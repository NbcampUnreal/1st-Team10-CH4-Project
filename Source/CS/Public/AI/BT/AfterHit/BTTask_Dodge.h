// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_Dodge.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_Dodge : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTTask_Dodge();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector ShouldDodgeKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

};