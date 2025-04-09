// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_DecideAttackType.generated.h"


UCLASS()
class CS_API UBTService_DecideAttackType : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_DecideAttackType();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector AttackTypeKey;
};