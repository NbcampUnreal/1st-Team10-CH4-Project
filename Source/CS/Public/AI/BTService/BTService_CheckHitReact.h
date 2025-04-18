
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CheckHitReact.generated.h"


UCLASS()
class CS_API UBTService_CheckHitReact : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_CheckHitReact();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ShouldBlockKey;
};
