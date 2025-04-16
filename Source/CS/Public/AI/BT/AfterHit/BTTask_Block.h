
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BTTask_Block.generated.h"


UCLASS()
class CS_API UBTTask_Block : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
public:
	UBTTask_Block(FObjectInitializer const& ObjectInitializer);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	void OnBlockMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldBlockKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsPlayerAttackingKey;
	
	int BlockCount;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	int Count;
	
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<AAIBaseCharacter> CachedCharacter;
	FOnMontageEnded MontageEndDelegate;
};
