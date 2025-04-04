#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIBaseController.generated.h"

UCLASS()
class CS_API AAIBaseController : public AAIController
{
    GENERATED_BODY()

public:
    explicit AAIBaseController(FObjectInitializer const& FObjectInitializer);
    
protected:
    virtual void OnPossess(APawn* InPawn) override;

private:
    class UAISenseConfig_Sight* SenseConfig;
    
    UFUNCTION()
    void SetupPerceptionSystem();

    UFUNCTION()
    void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
