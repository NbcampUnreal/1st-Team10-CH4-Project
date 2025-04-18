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
	float MinRange = 130.f; 
	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxRange = 200.f;   
	UPROPERTY(EditAnywhere, Category = "AI")
	float Chance = 0.6f;      
	
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector RangeComboKey;
};
