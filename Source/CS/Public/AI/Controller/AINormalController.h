#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AINormalController.generated.h"

UCLASS()
class CS_API AAINormalController : public AAIBaseController
{
	GENERATED_BODY()

public:
	AAINormalController(FObjectInitializer const& FObjectInitializer);
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus) override;
	virtual void SetupPerceptionSystem() override;
};
