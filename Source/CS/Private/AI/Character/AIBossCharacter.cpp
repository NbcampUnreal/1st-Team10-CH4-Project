#include "AI/Character/AIBossCharacter.h"
#include "NavigationSystem.h"
#include "AI/Controller/AIBossController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CSCombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

AAIBossCharacter::AAIBossCharacter()
{
	AIControllerClass = AAIBossController::StaticClass();
	Tags.Add(FName("Boss"));
}

void AAIBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

int AAIBossCharacter::Block_Implementation()
{
	if (!CombatComponent) return 0;
	if (bIsBlocking == true) return 0;
	bIsBlocking = true;
	if (!BlockMontage) return 0;
	if (BlockMontage)
	{
		PlayAnimMontage(BlockMontage);
		StopMovement();
	}
	return 1;
}

void AAIBossCharacter::StopBlock()
{
	bIsBlocking = false;
	ResumeMovement();
	GetWorldTimerManager().ClearTimer(BlockTimerHandle);
}

int AAIBossCharacter::Dodge_Implementation(AActor* Attacker)
{
	Dodge_StartDash(Attacker);
	return 1;
}

void AAIBossCharacter::Dodge_StartDash(AActor* Attacker)
{
	if (!Attacker) return;
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();
	
	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector AwayDir = FVector(0.f, YDir, 0.f);
	
	float DashStrength = 900.f;
	LaunchCharacter(AwayDir * DashStrength, true, true);
	
	FTimerHandle DodgeMoveTimerHandle;
	GetWorldTimerManager().SetTimer(
		DodgeMoveTimerHandle,
		FTimerDelegate::CreateUObject(this, &AAIBossCharacter::Dodge_MoveToSafeZone, Attacker),
		0.25f, false
	);
}


void AAIBossCharacter::Dodge_MoveToSafeZone(AActor* Attacker)
{
	ResumeMovement(); 
	if (!Attacker) return;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}
	
	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();
	
	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector RunDirection = FVector(0.f, YDir, 0.f);

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys)
	{
		return;
	}
	
	FNavLocation NavLocation;

	const float MinDistance = 200.f;
	const float MaxDistance = 400.f;
	const int MaxAttempts = 10;
	for (int i = 0; i < MaxAttempts; ++i)
	{
		FVector BasePoint = MyLocation + RunDirection * MinDistance;

		if (NavSys->GetRandomReachablePointInRadius(BasePoint, MaxDistance, NavLocation))
		{
			float ActualDistance = FVector::Dist2D(MyLocation, NavLocation.Location);

			if (ActualDistance >= MinDistance)
			{
				NavLocation.Location.X = MyLocation.X;
				NavLocation.Location.Z = MyLocation.Z;
				break;
			}
		}
	}
	AIController->MoveToLocation(NavLocation.Location, -1.f, true);
}

int AAIBossCharacter::RunAway_Implementation(AActor* Attacker)
{
	ResumeMovement();
	if (!Attacker) return 0;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return 0;
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (AnimInstance->IsAnyMontagePlaying())
		{
			AnimInstance->StopAllMontages(0.1f);
		}
	}

	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();

	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector RunDirection = FVector(0.f, YDir, 0.f);

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSys) return 0;

	FNavLocation NavLocation;

	const float MinDistance = 200.f;
	const float MaxDistance = 400.f;
	const int MaxAttempts = 10;
	bool bFoundValid = false;

	for (int i = 0; i < MaxAttempts; ++i)
	{
		FVector BasePoint = MyLocation + RunDirection * MinDistance;

		if (NavSys->GetRandomReachablePointInRadius(BasePoint, MaxDistance, NavLocation))
		{
			float ActualDistance = FVector::Dist2D(MyLocation, NavLocation.Location);
			if (ActualDistance >= MinDistance)
			{
				NavLocation.Location.X = MyLocation.X;
				NavLocation.Location.Z = MyLocation.Z;

				bFoundValid = true;
				break;
			}
		}
	}

	if (!bFoundValid)
	{
		return 0;
	}

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(NavLocation.Location);
	MoveRequest.SetAcceptanceRadius(5.f);
	AIController->MoveTo(MoveRequest);

	return 1;
}


