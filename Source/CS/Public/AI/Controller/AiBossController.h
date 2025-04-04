#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIBossController.generated.h"

UCLASS()
class CS_API AAIBossController : public AAIBaseController
{
	GENERATED_BODY()

public:
	AAIBossController(FObjectInitializer const& FObjectInitializer);
	
protected:
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY()
	UBehaviorTree* Tree;
};
