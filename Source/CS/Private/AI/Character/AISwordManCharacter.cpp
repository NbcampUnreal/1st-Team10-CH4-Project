#include "AI/Character/AISwordManCharacter.h"

#include "AI/Controller/AISwordManController.h"

AAISwordManCharacter::AAISwordManCharacter()
{
	AIControllerClass = AAISwordManController::StaticClass();
	Tags.Add(FName("SwordMan"));
}


void AAISwordManCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AAISwordManCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAISwordManCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
FComboAttackData AAISwordManCharacter::GetFirstAttackData() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;

	if (CurrentTime - LastPunchTime > ComboResetCooldown)
	{
		CurrentPunchIndex = 0;
	}
	else
	{
		CurrentPunchIndex = (CurrentPunchIndex + 1) % 2;
	}
	LastPunchTime = CurrentTime;

	FComboAttackData AttackData;

	switch (CurrentPunchIndex)
	{
	case 0:
		AttackData.SectionName = FName("Light1");
		AttackData.Damage = NomalDamage;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	case 1:
		AttackData.SectionName = FName("Light2");
		AttackData.Damage = NomalDamage;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	default:
		AttackData.SectionName = FName("Light1");
		AttackData.Damage = NomalDamage;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	}

	return AttackData;
}


FComboAttackData AAISwordManCharacter::GetSecondAttackData() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;

	if (CurrentTime - LastPunchTime > ComboResetCooldown)
	{
		CurrentPunchIndex = 0;
	}
	else
	{
		CurrentPunchIndex = (CurrentPunchIndex + 1) % 2;
	}
	LastPunchTime = CurrentTime;

	FComboAttackData AttackData;

	switch (CurrentPunchIndex)
	{
	case 0:
		AttackData.SectionName = FName("Attack1");
		AttackData.Damage = NomalDamage;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	case 1:
		AttackData.SectionName = FName("Attack2");
		AttackData.Damage = NomalDamage;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	default:
		AttackData.SectionName = FName("Attack1");
		AttackData.Damage = NomalDamage;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	}

	return AttackData;
}

FComboAttackData AAISwordManCharacter::GetRangeComboAttackData() const
{
	FComboAttackData AttackData;

	AttackData.SectionName = FName("Counter");
	AttackData.Damage = ComboDamage;
	AttackData.DType = ELaunchTypes::EDT_Launch;
	
	return AttackData;
}
FComboAttackData AAISwordManCharacter::GetLowComboAttackData() const
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

	FComboAttackData AttackData;

	switch (CurrentPunchIndex)
	{
	case 0:
		AttackData.SectionName = FName("Attack1");
		AttackData.Damage = SwoardComboDamage;
		AttackData.DType = ELaunchTypes::EDT_Launch;
		break;
	case 1:
		AttackData.SectionName = FName("Attack2");
		AttackData.Damage = SwoardComboDamage;
		AttackData.DType = ELaunchTypes::EDT_Launch;
		break;
	case 2:
		AttackData.SectionName = FName("Attack3");
		AttackData.Damage = SwoardComboDamage;
		AttackData.DType = ELaunchTypes::EDT_Launch;
		break;
	default:
		AttackData.SectionName = FName("Attack1");
		AttackData.Damage = SwoardComboDamage;
		AttackData.DType = ELaunchTypes::EDT_Launch;
		break;
	}

	return AttackData;
}

int AAISwordManCharacter::FirstAttack_Implementation()
{
	FComboAttackData AttackData = GetFirstAttackData();
	FName tracestart = "hand_r_socket";
	FName traceend = "hand_r_trace";
	AI_Attack(GetFirstAttackMontage(), AttackData, tracestart, traceend);
	return 1;
}

int AAISwordManCharacter::SecondAttack_Implementation()
{
	FComboAttackData AttackData = GetSecondAttackData();
	FName tracestart = "hand_r_socket";
	FName traceend = "hand_r_trace";
	AI_Attack(GetSecondAttackMontage(), AttackData, tracestart, traceend);
	
	return 1;
}
int AAISwordManCharacter::LowComboAttack_Implementation()
{
	FComboAttackData AttackData = GetLowComboAttackData();
	FName tracestart = "hand_r_socket";
	FName traceend = "hand_r_trace";
	AI_Attack(GetLowComboAttackMontage(), AttackData, tracestart, traceend);
	
	return 1;
}
int AAISwordManCharacter::RangeComboAttack_Implementation()
{
	FComboAttackData AttackData = GetRangeComboAttackData();
	FName tracestart = FName("hand_r_socket");
	FName traceend = FName("hand_r_trace");
	AI_Attack(GetRangeComboAttackMontage(), AttackData, tracestart, traceend);
	
	return 1;
}
