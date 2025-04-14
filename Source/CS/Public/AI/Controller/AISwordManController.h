#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "AISwordManController.generated.h"

UCLASS()
class CS_API AAISwordManController : public AAIBaseController
{
	GENERATED_BODY()

public:
	AAISwordManController(FObjectInitializer const& FObjectInitializer);;
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupPerceptionSystem() override;
	virtual void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus) override;

};
