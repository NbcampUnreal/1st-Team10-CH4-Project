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
}

void AAIBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentStance = ECharacterStance::Standing;
}

void AAIBossCharacter::JumpAction()
{
		Jump();
		CurrentStance = ECharacterStance::Jumping;

		
		FTimerHandle JumpEndHandle;
		GetWorldTimerManager().SetTimer(JumpEndHandle, this, &AAIBossCharacter::EndJump, 1.0f, false);
	
}

void AAIBossCharacter::EndJump()
{
	CurrentStance = ECharacterStance::Standing;
}

void AAIBossCharacter::CrouchAction()
{
	Crouch();
	CurrentStance = ECharacterStance::Crouching;


	FTimerHandle CrouchEndHandle;
	GetWorldTimerManager().SetTimer(CrouchEndHandle, this, &AAIBossCharacter::EndCrouch, 1.0f, false);
}

void AAIBossCharacter::EndCrouch()
{
	UnCrouch(); 
	CurrentStance = ECharacterStance::Standing;
}


void AAIBossCharacter::StartComboAttack()
{
	if (!CombatComponent) return;

	TArray<FName> ComboSections = { "Punch1", "Punch2", "Punch3" };
	if (CurrentComboIndex >= ComboSections.Num())
	{
		CurrentComboIndex = 0;
	}

	UAnimMontage* Montage = GetPunchMontage();
	if (Montage)
	{
		CombatComponent->PerformAttack(Montage, ComboSections[CurrentComboIndex]);
		CurrentComboIndex++;
		GetWorldTimerManager().SetTimer(ComboResetTimerHandle, this, &AAIBossCharacter::ResetCombo, 2.f);
	}
}

void AAIBossCharacter::ResetCombo()
{
	CurrentComboIndex = 0;
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
		
		UE_LOG(LogTemp, Warning, TEXT("Block Montage:"));
	}
	return 1;
}

int32 AAIBossCharacter::Dodge_Implementation(AActor* Attacker)
{
	Dodge_StartDash(Attacker);
	return 1;
}

void AAIBossCharacter::Dodge_StartDash(AActor* Attacker)
{
	if (!Attacker) return;

	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();

	// Y축 방향으로만 반응
	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector AwayDir = FVector(0.f, YDir, 0.f); // X/Z 고정, Y만 방향성 반영

	float DashStrength = 1200.f; // 대시 속도
	LaunchCharacter(AwayDir * DashStrength, true, true);

	UE_LOG(LogTemp, Log, TEXT("회피 대시 시작 - 방향: %s"), *AwayDir.ToString());

	FTimerHandle DodgeMoveTimerHandle;
	GetWorldTimerManager().SetTimer(DodgeMoveTimerHandle, FTimerDelegate::CreateUObject(this, &AAIBossCharacter::Dodge_MoveToSafeZone, Attacker), 0.4f, false);
}

void AAIBossCharacter::Dodge_MoveToSafeZone(AActor* Attacker)
{
	if (!Attacker) return;

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController) return;

	FVector MyLocation = GetActorLocation();
	FVector AttackerLocation = Attacker->GetActorLocation();

	// Y축 방향으로만 이동
	float YDir = FMath::Sign(MyLocation.Y - AttackerLocation.Y);
	FVector AwayDir = FVector(0.f, YDir, 0.f); // 횡스크롤 방향 유지

	float DodgeDistance = 300.f;
	FVector DodgeTargetLocation = MyLocation + AwayDir * DodgeDistance;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	FNavLocation NavLocation;

	if (!NavSys || !NavSys->ProjectPointToNavigation(DodgeTargetLocation, NavLocation))
	{
		UE_LOG(LogTemp, Error, TEXT("회피 목적지 NavMesh 바깥! 위치: %s"), *DodgeTargetLocation.ToString());
		return;
	}

	auto Result = AIController->MoveToLocation(NavLocation.Location, -1.f, true);

	if (Result == EPathFollowingRequestResult::RequestSuccessful)
	{
		UE_LOG(LogTemp, Log, TEXT("회피 MoveToLocation 성공! -> %s"), *NavLocation.Location.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("회피 MoveToLocation 실패! Result: %d"), (int32)Result);
	}
}


void AAIBossCharacter::StopBlock()
{
	bIsBlocking = false;
	
	GetWorldTimerManager().ClearTimer(BlockTimerHandle);
}



