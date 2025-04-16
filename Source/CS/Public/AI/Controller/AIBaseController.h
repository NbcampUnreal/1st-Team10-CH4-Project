#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIBaseController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class CS_API AAIBaseController : public AAIController
{
    GENERATED_BODY()

public:
    explicit AAIBaseController(FObjectInitializer const& FObjectInitializer);
    void StartLogicAI();
    
protected:
    virtual void OnPossess(APawn* InPawn) override;
    UFUNCTION()
    virtual void SetupPerceptionSystem();
    UFUNCTION()
    virtual void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
    UPROPERTY()
    class UAISenseConfig_Sight* SenseConfig;
    
};
