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
			AttackData.Damage = NomalDamage;
			break;
		case 1:
			AttackData.SectionName = "Light2";
			AttackData.Damage = NomalDamage;
			break;
		default:
			AttackData.SectionName = "Light1";
			AttackData.Damage = NomalDamage;
			break;
		}
	}
	else
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = "Punch1";
			AttackData.Damage = NomalDamage;
			break;
		case 1:
			AttackData.SectionName = "Punch2";
			AttackData.Damage = NomalDamage;
			break;
		case 2:
			AttackData.SectionName = "Punch3";
			AttackData.Damage = NomalDamage;
			break;
		default:
			AttackData.SectionName = "Punch1";
			AttackData.Damage = NomalDamage;
			break;
		}
	}

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
			: (CurrentPunchIndex + 1) % 3;
	}

	LastPunchTime = CurrentTime;

	if (bUseSwordManStyle)
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = FName("Attack1");
			AttackData.Damage = NomalDamage;
			break;
		case 1:
			AttackData.SectionName = FName("Attack2");
			AttackData.Damage = NomalDamage;
			break;
		default:
			AttackData.SectionName = FName("Attack1");
			AttackData.Damage = NomalDamage;
			break;
		}
	}
	else
	{
		switch (CurrentPunchIndex)
		{
		case 0:
			AttackData.SectionName = FName("Kick1");
			AttackData.Damage = NomalDamage;
			break;
		case 1:
			AttackData.SectionName = FName("Kick2");
			AttackData.Damage = NomalDamage;
			break;
		case 2:
			AttackData.SectionName = FName("Kick3");
			AttackData.Damage = NomalDamage;
			break;
		default:
			AttackData.SectionName = FName("Kick1");
			AttackData.Damage = NomalDamage;
			break;
		}
	}

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
			AttackData.Damage = SwoardComboDamage;
			break;
		case 1:
			AttackData.SectionName = FName("Attack2");
			AttackData.Damage = SwoardComboDamage;
			break;
		case 2:
			AttackData.SectionName = FName("Attack3");
			AttackData.Damage = SwoardComboDamage;
			break;
		default:
			AttackData.SectionName = FName("Attack1");
			AttackData.Damage = SwoardComboDamage;
			break;
		}
	}
	else
	{
		AttackData.SectionName = FName("Default");
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
		AttackData.Damage = ComboDamage;
	}
	else
	{
		AttackData.SectionName = FName("Default");
		AttackData.Damage = ComboDamage;
	}

	AttackData.DType = ELaunchTypes::EDT_Launch;
	return AttackData;
}


int AAIBossCharacter::FirstAttack_Implementation()
{
	FComboAttackData AttackData = GetFirstAttackData();
	
	FName tracestart;
	FName traceend;
	
	if (bUseSwordManStyle)
	{
		tracestart = "hand_r_socket";
		traceend = "hand_r_trace";
		AI_Attack(GetFirstAttackMontage2(), AttackData, tracestart, traceend);
	}
	else
	{
		tracestart = "hand_r";
		traceend = "hand_l";
		AI_Attack(GetFirstAttackMontage(), AttackData, tracestart, traceend);
	}
	return 1;
}

int AAIBossCharacter::SecondAttack_Implementation()
{
	FComboAttackData AttackData = GetSecondAttackData();
	FName tracestart;
	FName traceend;
	
	if (bUseSwordManStyle)
	{
		tracestart = "hand_r_socket";
		traceend = "hand_r_trace";
		AI_Attack(GetSecondAttackMontage2(), AttackData, tracestart, traceend);
	}
	else
	{
		tracestart = "foot_l";
		traceend = "foot_r";
		AI_Attack(GetSecondAttackMontage(), AttackData, tracestart, traceend);
	}
	
	return 1;
}
int AAIBossCharacter::LowComboAttack_Implementation()
{
	FComboAttackData AttackData = GetLowComboAttackData();
	FName tracestart;
	FName traceend;
	
	if (bUseSwordManStyle)
	{
		tracestart = "hand_r_socket";
		traceend = "hand_r_trace";
		AI_Attack(GetLowComboAttackMontage2(), AttackData, tracestart, traceend);
	}
	else
	{
		tracestart = "foot_l";
		traceend = "foot_r";
		AI_Attack(GetLowComboAttackMontage(), AttackData, tracestart, traceend);
	}
	
	
	return 1;
}
int AAIBossCharacter::RangeComboAttack_Implementation()
{
	FComboAttackData AttackData = GetRangeComboAttackData();
	FName tracestart;
	FName traceend;
	StopMovement();
	if (bUseSwordManStyle)
	{
		tracestart = "hand_r_socket";
		traceend = "hand_r_trace";
		AI_Attack(GetRangeComboAttackMontage2(), AttackData, tracestart, traceend);
	}
	else
	{
		tracestart = "RightFistSocket";
		traceend = "RightFistSocketEnd";
		AI_Attack(GetRangeComboAttackMontage(), AttackData, tracestart, traceend);
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
