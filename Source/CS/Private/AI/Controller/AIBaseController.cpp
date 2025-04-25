#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Characters/CSPlayerCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AAIBaseController::AAIBaseController(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	bStartAILogicOnPossess = false;
	
	SenseConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component")));
}


void AAIBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAIBaseController::SetupPerceptionSystem();

	if (AAIBaseCharacter* const NPC = Cast<AAIBaseCharacter>(InPawn))
	{
		if (UBehaviorTree* const Tree = NPC->GetBehaviorTree())
		{
			UBlackboardComponent* BB;
			if (UseBlackboard(Tree->BlackboardAsset, BB))
			{
				Blackboard = BB;
				
				RunBehaviorTree(Tree);
			}
		}
	}
}




void AAIBaseController::SetupPerceptionSystem()
{
	if (SenseConfig)
	{
		SenseConfig->SightRadius = 500.0f;
		SenseConfig->LoseSightRadius = SenseConfig->SightRadius + 100.f;
		SenseConfig->PeripheralVisionAngleDegrees = 270.0f;
		SenseConfig->SetMaxAge(6.0f);
		SenseConfig->AutoSuccessRangeFromLastSeenLocation = 520.0f;
		SenseConfig->DetectionByAffiliation.bDetectEnemies = true;
		SenseConfig->DetectionByAffiliation.bDetectFriendlies = true;
		SenseConfig->DetectionByAffiliation.bDetectNeutrals = true;

		GetPerceptionComponent()->SetDominantSense(*SenseConfig->GetSenseImplementation());
		if (!GetPerceptionComponent()->OnTargetPerceptionUpdated.IsAlreadyBound(this, &AAIBaseController::OnTargetDetected))
		{
			GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AAIBaseController::OnTargetDetected);
		}
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

void AAIBaseController::StartLogicAI()
{
	if (BrainComponent)
	{
		BrainComponent->StartLogic(); 
	}
}

