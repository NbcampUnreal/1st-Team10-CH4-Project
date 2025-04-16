#include "AI/Controller/AIFighterController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CSPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AAIFighterController::AAIFighterController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
	AAIFighterController::SetupPerceptionSystem();
}

void AAIFighterController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
}

void AAIFighterController::SetupPerceptionSystem()
{
	SenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config")); 
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
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIFighterController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SenseConfig);
	}
}

void AAIFighterController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	Super::OnTargetDetected(Actor, Stimulus);
}