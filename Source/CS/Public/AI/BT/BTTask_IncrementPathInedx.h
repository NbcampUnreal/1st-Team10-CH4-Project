#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_IncrementPathInedx.generated.h"

UCLASS()
class CS_API UBTTask_IncrementPathInedx : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_IncrementPathInedx(FObjectInitializer const& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	enum class EDirectionType {Forward, Reverse};

	EDirectionType Direction = EDirectionType::Forward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", Meta = (AllowPrivateAccess=true))
	bool bBiDirectional = false;
};
