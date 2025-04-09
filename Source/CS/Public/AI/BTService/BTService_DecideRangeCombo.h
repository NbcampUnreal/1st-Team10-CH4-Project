#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_DecideRangeCombo.generated.h"

UCLASS()
class CS_API UBTService_DecideRangeCombo : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_DecideRangeCombo();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category = "AI")
	float MinRange = 130.f;    // 최소 거리
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxRange = 200.f;    // 최대 거리
	UPROPERTY(EditAnywhere, Category = "AI")
	float Chance = 0.6f;       // 발동 확률
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RangeComboKey;
};
