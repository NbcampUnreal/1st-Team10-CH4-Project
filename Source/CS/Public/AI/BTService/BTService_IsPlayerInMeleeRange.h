
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_IsPlayerInMeleeRange.generated.h"


UCLASS()
class CS_API UBTService_IsPlayerInMeleeRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_IsPlayerInMeleeRange();
	void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="AI", meta=(AllowPrivateAccess=true))
	float MeleeRange = 2.5f;
};
