// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_MeleeNormalAttack.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_MeleeNormalAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
		
public:
	explicit UBTTask_MeleeNormalAttack(FObjectInitializer const& ObjectInitializer);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	FTimerHandle AttackCooldownTimerHandle;

	// 공격 애니메이션 종료 감지 후 Task 종료
	void FinishLatentTaskEarly(UBehaviorTreeComponent* OwnerComp);

	// 애니메이션 끝났는지 확인
	bool MontageHasfinished(class AAIBaseCharacter* const AI);
};

