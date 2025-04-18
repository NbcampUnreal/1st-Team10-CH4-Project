#include "AI/Character/AIBossCharacter.h"
#include "NavigationSystem.h"
#include "AI/Controller/AIBossController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CSAttributeComponent.h"
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

	if (!PatrolPath)
	{
		TArray<AActor*> FoundPaths;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolPath::StaticClass(), FoundPaths);

		for (AActor* Actor : FoundPaths)
		{
			if (Actor->ActorHasTag(FName("BossPatrol")))
			{
				PatrolPath = Cast<APatrolPath>(Actor);
				break;
			}
		}
	}
}




void AAIBossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bUseSwordManStyle && AttributeComponent && AttributeComponent->GetHealthPercent() <= 0.5f)
	{
		bUseSwordManStyle = true;
		HandlePhase2Transition();
	}
}

FComboAttackData AAIBossCharacter::GetFirstAttackData() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;
	FComboAttackData AttackData;
	
	if (CurrentTime - LastPunchTime > ComboResetCooldown)
	{
		CurrentPunchIndex = 0;
	}
	else
	{
		CurrentPunchIndex = bUseSwordManStyle
			? (CurrentPunchIndex + 1) % 2
			: (CurrentPunchIndex + 1) % 3;
	}

	LastPunchTime = CurrentTime;

	if (bUseSwordManStyle)
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = "Light1";
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			break;
		case 1:
			AttackData.SectionName = "Light2";
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			break;
		default:
			AttackData.SectionName = "Light1";
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			break;
		}
	}
	else
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = "Punch1";
			AttackData.TraceStart = FName("hand_r");
			AttackData.TraceEnd = FName("hand_l");
			
			break;
		case 1:
			AttackData.SectionName = "Punch2";
			AttackData.TraceStart = FName("hand_r");
			AttackData.TraceEnd = FName("hand_l");
			
			break;
		case 2:
			AttackData.SectionName = "Punch3";
			AttackData.TraceStart = FName("hand_r");
			AttackData.TraceEnd = FName("hand_l");
			
			break;
		default:
			AttackData.SectionName = "Punch1";
			AttackData.TraceStart = FName("hand_r");
			AttackData.TraceEnd = FName("hand_l");
			
			break;
		}
	}
	AttackData.Damage = NomalDamage;
	AttackData.DType = ELaunchTypes::EDT_Nomal;
	return AttackData;
}


FComboAttackData AAIBossCharacter::GetSecondAttackData() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;
	FComboAttackData AttackData;
	
	if (CurrentTime - LastPunchTime > ComboResetCooldown)
	{
		CurrentPunchIndex = 0;
	}
	else
	{
		CurrentPunchIndex = bUseSwordManStyle
			? (CurrentPunchIndex + 1) % 2
			: (CurrentPunchIndex + 1) % 4;
	}

	LastPunchTime = CurrentTime;

	if (bUseSwordManStyle)
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = FName("Attack1");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			break;
		case 1:
			AttackData.SectionName = FName("Attack2");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			break;
		default:
			AttackData.SectionName = FName("Attack1");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			break;
		}
	}
	else
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = FName("Kick1");
			AttackData.TraceStart = FName("thigh_r");
			AttackData.TraceEnd = FName("foot_r");
			break;
		case 1:
			AttackData.SectionName = FName("Kick2");
			AttackData.TraceStart = FName("thigh_l");
			AttackData.TraceEnd = FName("foot_l");
			break;
		case 2:
			AttackData.SectionName = FName("Kick3");
			AttackData.TraceStart = FName("foot_l");
			AttackData.TraceEnd = FName("thigh_l");
			break;
		case 3:
			AttackData.SectionName = FName("Kick4");
			AttackData.TraceStart = FName("hand_r");
			AttackData.TraceEnd = FName("hand_l");
			break;
		default:
			AttackData.SectionName = FName("Kick1");
			AttackData.TraceStart = FName("foot_l");
			AttackData.TraceEnd = FName("thigh_l");
			break;
		}
	}
	AttackData.Damage = NomalDamage;
	AttackData.DType = ELaunchTypes::EDT_Nomal;
	return AttackData;
}

FComboAttackData AAIBossCharacter::GetLowComboAttackData() const
{
	FComboAttackData AttackData;
	if (bUseSwordManStyle)
	{
		const float CurrentTime = GetWorld()->TimeSeconds;

		if (CurrentTime - LastPunchTime > ComboResetCooldown)
		{
			CurrentPunchIndex = 0;
		}
		else
		{
			CurrentPunchIndex = (CurrentPunchIndex + 1) % 3;
		}
		LastPunchTime = CurrentTime;
		
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = FName("Attack1");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			AttackData.Damage = SwoardComboDamage;
			break;
		case 1:
			AttackData.SectionName = FName("Attack2");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			AttackData.Damage = SwoardComboDamage;
			break;
		case 2:
			AttackData.SectionName = FName("Attack3");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			AttackData.Damage = SwoardComboDamage;
			break;
		default:
			AttackData.SectionName = FName("Attack1");
			AttackData.TraceStart = FName("hand_r_socket");
			AttackData.TraceEnd = FName("hand_r_trace");
			AttackData.Damage = SwoardComboDamage;
			break;
		}
	}
	else
	{
		AttackData.SectionName = FName("Default");
		AttackData.TraceStart = FName("foot_l");
		AttackData.TraceEnd = FName("foot_r");
		AttackData.Damage = ComboDamage;
	}
	
	AttackData.DType = ELaunchTypes::EDT_Launch;
	return AttackData;
}

FComboAttackData AAIBossCharacter::GetRangeComboAttackData() const
{
	FComboAttackData AttackData;
	if (bUseSwordManStyle)
	{
		AttackData.SectionName = FName("Counter");
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		AttackData.Damage = ComboDamage;
	}
	else
	{
		AttackData.SectionName = FName("Default");
		AttackData.TraceStart = FName("RightFistSocket");
		AttackData.TraceEnd = FName("RightFistSocketEnd");
		AttackData.Damage = ComboDamage;
	}

	AttackData.DType = ELaunchTypes::EDT_Launch;
	return AttackData;
}


int AAIBossCharacter::FirstAttack_Implementation()
{
	FComboAttackData AttackData = GetFirstAttackData();
	
	if (bUseSwordManStyle)
	{
		AI_Attack(GetFirstAttackMontage2(), AttackData);
	}
	else
	{
		AI_Attack(GetFirstAttackMontage(), AttackData);
	}
	return 1;
}

int AAIBossCharacter::SecondAttack_Implementation()
{
	FComboAttackData AttackData = GetSecondAttackData();
	
	if (bUseSwordManStyle)
	{
		AI_Attack(GetSecondAttackMontage2(), AttackData);
	}
	else
	{
		AI_Attack(GetSecondAttackMontage(), AttackData);
	}
	
	return 1;
}
int AAIBossCharacter::LowComboAttack_Implementation()
{
	FComboAttackData AttackData = GetLowComboAttackData();

	if (bUseSwordManStyle)
	{
		AI_Attack(GetLowComboAttackMontage2(), AttackData);
	}
	else
	{
		AI_Attack(GetLowComboAttackMontage(), AttackData);
	}
	
	return 1;
}
int AAIBossCharacter::RangeComboAttack_Implementation()
{
	FComboAttackData AttackData = GetRangeComboAttackData();
	StopMovement();
	if (bUseSwordManStyle)
	{
		AI_Attack(GetRangeComboAttackMontage2(), AttackData);
	}
	else
	{
		AI_Attack(GetRangeComboAttackMontage(), AttackData);
	}
	
	return 1;
}

int AAIBossCharacter::Block_Implementation()
{
	if (!CombatComponent) return 0;
	if (bIsBlocking == true) return 0;
	bIsBlocking = true;
	if (!BlockMontage) return 0;
	if (BlockMontage)
	{
		if (bUseSwordManStyle)
		{
			PlayAnimMontage(BlockMontage2);
			StopMovement();
		}
		else
		{
			PlayAnimMontage(BlockMontage);
			StopMovement();
		}
	}
	return 1;
}

void AAIBossCharacter::HandlePhase2Transition() const
{
	if (SwordActorClass)
	{
		FActorSpawnParameters SpawnParams;
		AActor* SpawnedSword = GetWorld()->SpawnActor<AActor>(SwordActorClass, SpawnParams);
		
		if (SpawnedSword)
		{
			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
			SpawnedSword->AttachToComponent(GetMesh(), AttachRules, FName("hand_r_trace"));
		}
	}
}
