#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "AIFighterController.generated.h"

UCLASS()
class CS_API AAIFighterController : public AAIBaseController
{
	GENERATED_BODY()

public:
	AAIFighterController(FObjectInitializer const& FObjectInitializer);;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupPerceptionSystem() override;
	virtual void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus) override;
	
};
