// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService/BTService_AnalyzeSituation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Components/CSAttributeComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTService_AnalyzeSituation::UBTService_AnalyzeSituation()
{
	NodeName = TEXT("Analyze Situation");
	Interval = 0.2f;

	ShouldJumpKey.SelectedKeyName = "ShouldJump";
	ShouldCrouchKey.SelectedKeyName = "ShouldCrouch";
	ShouldRunAwayKey.SelectedKeyName = "ShouldRunAway";
}

void UBTService_AnalyzeSituation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return;

	if (BB->GetValueAsBool(FName("IsBusy")))
		return;

	if (auto* AIPawn = Cast<AAIBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		auto* Player = UGameplayStatics::GetPlayerCharacter(AIPawn->GetWorld(), 0);
		if (!Player) return;

		const float Distance = FVector::Dist(AIPawn->GetActorLocation(), Player->GetActorLocation());
		
		BB->SetValueAsBool(ShouldJumpKey.SelectedKeyName, false);
		BB->SetValueAsBool(ShouldCrouchKey.SelectedKeyName, false);
		BB->SetValueAsBool(ShouldRunAwayKey.SelectedKeyName, false);

	
		int32 Roll = FMath::RandRange(0, 99);

		
		if (Distance < 300.f && Roll < 30)
		{
			BB->SetValueAsBool(ShouldRunAwayKey.SelectedKeyName, true);
		}
		else if (Roll < 60) 
		{
			BB->SetValueAsBool(ShouldJumpKey.SelectedKeyName, true);
		}
		else if (Roll < 80) 
		{
			BB->SetValueAsBool(ShouldCrouchKey.SelectedKeyName, true);
		}
	}
}



