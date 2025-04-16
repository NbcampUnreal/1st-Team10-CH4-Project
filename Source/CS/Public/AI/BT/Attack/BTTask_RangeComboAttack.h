// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_RangeComboAttack.generated.h"

/**
 * 
 */
UCLASS()
class CS_API UBTTask_RangeComboAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_RangeComboAttack(FObjectInitializer const& ObjectInitializer);
	
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	bool MontageHasfinished(AAIBaseCharacter* AI);
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
};
