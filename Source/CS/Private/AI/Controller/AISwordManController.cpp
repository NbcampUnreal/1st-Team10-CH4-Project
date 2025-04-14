#include "AI/Controller/AISwordManController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CSPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AAISwordManController::AAISwordManController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
	AAISwordManController::SetupPerceptionSystem();
}

void AAISwordManController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void AAISwordManController::SetupPerceptionSystem()
{
	SenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config")); 
	if (SenseConfig && GetPerceptionComponent())
	{
		SenseConfig->SightRadius = 1500.0f;
		SenseConfig->LoseSightRadius = SenseConfig->SightRadius + 100.0f;
		SenseConfig->PeripheralVisionAngleDegrees = 360.0f;
		SenseConfig->SetMaxAge(2.0f);
		SenseConfig->AutoSuccessRangeFromLastSeenLocation = 1600.0f;

		SenseConfig->DetectionByAffiliation.bDetectEnemies = true;
		SenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SenseConfig->DetectionByAffiliation.bDetectNeutrals = true;
		
		GetPerceptionComponent()->SetDominantSense(*SenseConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAISwordManController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SenseConfig);
	}
}


void AAISwordManController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (auto* const character = Cast<ACSPlayerCharacter>(Actor))
	{
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());
	}
}