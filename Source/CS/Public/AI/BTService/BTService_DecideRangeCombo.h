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
	int32 ComboChance = 20;


	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RangeComboKey;


	UPROPERTY(EditAnywhere, Category = "AI")
	float ComboTriggerDistance = 160.0f;
};
