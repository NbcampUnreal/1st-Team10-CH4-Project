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
	/*bStartAILogicOnPossess = false;*/
	bStartAILogicOnPossess = true;
}

void AAIBaseController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AAIBaseCharacter* const NPC = Cast<AAIBaseCharacter>(InPawn))
	{
		if (UBehaviorTree* const Tree = NPC->GetBehaviorTree())
		{
			UBlackboardComponent* BB;
			UseBlackboard(Tree->BlackboardAsset, BB);
			Blackboard = BB;
			RunBehaviorTree(Tree);
			/*BehaviorTreeComponent = NewObject<UBehaviorTreeComponent>(this, TEXT("BTComponent"));
			BehaviorTreeComponent->RegisterComponent();*/
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

void AAIBaseController::StartLogicAI()
{
	/*if (BrainComponent)
	{
		BrainComponent->StartLogic(); 
	}*/
}

