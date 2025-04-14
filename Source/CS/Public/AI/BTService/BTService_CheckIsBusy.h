#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_CheckIsBusy.generated.h"

struct FBTIsBusyMemory
{
	float ElapsedBusyTime = 0.f;
};

UCLASS()
class CS_API UBTService_CheckIsBusy : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_CheckIsBusy();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override
	{
		return sizeof(FBTIsBusyMemory);
	}

private:
	static const FName IsBusyKeyName;
};
