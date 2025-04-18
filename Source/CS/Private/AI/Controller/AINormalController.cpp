#include "AI/Controller/AINormalController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AAINormalController::AAINormalController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
}

void AAINormalController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAINormalController::SetupPerceptionSystem();
}

void AAINormalController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	Super::OnTargetDetected(Actor, Stimulus);
}

void AAINormalController::SetupPerceptionSystem()
{
	Super::SetupPerceptionSystem();
}
