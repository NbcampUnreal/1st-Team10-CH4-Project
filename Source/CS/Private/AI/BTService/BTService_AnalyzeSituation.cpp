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
	bool bIsBusy = BB->GetValueAsBool(FName("IsBusy"));
	if (bIsBusy)
	{
		return;
	}
	bool bIsJumping = BB->GetValueAsBool(ShouldJumpKey.SelectedKeyName);
	bool bIsSitting = BB->GetValueAsBool(ShouldCrouchKey.SelectedKeyName);
	bool bIsRunning = BB->GetValueAsBool(ShouldRunAwayKey.SelectedKeyName);

	if (bIsJumping || bIsSitting || bIsRunning)
	{
		return;
	}
	
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	AAIBaseCharacter* AIPawn = Cast<AAIBaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Player || !AIPawn) return;

	float Distance = FVector::Dist(AIPawn->GetActorLocation(), Player->GetActorLocation());

	bool bShouldJump = (FMath::FRand() < 0.4f);
	bool bShouldCrouch = (FMath::FRand() < 0.1f);
	bool bShouldRunAway = (Distance < 300.f && FMath::FRand() < 0.5f);

	BB->SetValueAsBool(ShouldJumpKey.SelectedKeyName, bShouldJump);
	BB->SetValueAsBool(ShouldCrouchKey.SelectedKeyName, bShouldCrouch);
	BB->SetValueAsBool(ShouldRunAwayKey.SelectedKeyName, bShouldRunAway);
}

