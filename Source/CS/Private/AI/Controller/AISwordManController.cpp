#include "AI/Controller/AISwordManController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CSPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AAISwordManController::AAISwordManController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
}

void AAISwordManController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAISwordManController::SetupPerceptionSystem();
}

void AAISwordManController::SetupPerceptionSystem()
{
	Super::SetupPerceptionSystem();
	
	if (SenseConfig && GetPerceptionComponent())
	{
		SenseConfig->SightRadius = 1500.0f;
		SenseConfig->LoseSightRadius = SenseConfig->SightRadius + 100.0f;
		SenseConfig->PeripheralVisionAngleDegrees = 360.0f;
		SenseConfig->SetMaxAge(5.0f);
		SenseConfig->AutoSuccessRangeFromLastSeenLocation = 1600.0f;

		SenseConfig->DetectionByAffiliation.bDetectEnemies = true;
		SenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
		
		GetPerceptionComponent()->SetDominantSense(*SenseConfig->GetSenseImplementation());
		if (!GetPerceptionComponent()->OnTargetPerceptionUpdated.IsAlreadyBound(this,  &AAISwordManController::OnTargetDetected))
		{
			GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,  &AAISwordManController::OnTargetDetected);
		}
		GetPerceptionComponent()->ConfigureSense(*SenseConfig);
	}
}


void AAISwordManController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	Super::OnTargetDetected(Actor, Stimulus);
}