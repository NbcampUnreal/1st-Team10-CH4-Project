#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/CSPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AAIBaseController::AAIBaseController(FObjectInitializer const& FObjectInitializer): Super(FObjectInitializer)
{
	SetupPerceptionSystem();
}

void AAIBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AAIBaseCharacter* const npc = Cast<AAIBaseCharacter>(InPawn))
	{
		if (UBehaviorTree* const Tree = npc->GetBehaviorTree())
		{
			UBlackboardComponent* bbAI;
			UseBlackboard(Tree->BlackboardAsset,bbAI);
			Blackboard = bbAI;
			RunBehaviorTree(Tree);
		}
	}
}


void AAIBaseController::SetupPerceptionSystem()
{
	SenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Signt Config");
	if (SenseConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(
		TEXT("Perception Component")));
		SenseConfig->SightRadius = 500.0f;
		SenseConfig->LoseSightRadius = SenseConfig->SightRadius + 25.f;
		SenseConfig->PeripheralVisionAngleDegrees = 90.0f;
		SenseConfig->SetMaxAge(5.0f);
		SenseConfig->AutoSuccessRangeFromLastSeenLocation = 520.0f;
		SenseConfig->DetectionByAffiliation.bDetectEnemies = true;
		SenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SenseConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SenseConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIBaseController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SenseConfig);
		
		
	}
}


void AAIBaseController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (auto* const character = Cast<ACSPlayerCharacter>(Actor))
	{
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());
	}
}