#include "AI/Character/AISwordManCharacter.h"

AAISwordManCharacter::AAISwordManCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
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
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	case 1:
		AttackData.SectionName = FName("Light2");
		AttackData.Damage = 12.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	default:
		AttackData.SectionName = FName("Light1");
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Nomal;
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
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	case 1:
		AttackData.SectionName = FName("Attack2");
		AttackData.Damage = 12.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	default:
		AttackData.SectionName = FName("Attack1");
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Nomal;
		break;
	}

	return AttackData;
}

FComboAttackData AAISwordManCharacter::GetLowComboAttackData() const
{
	FComboAttackData AttackData;

	AttackData.SectionName = FName("Counter");
	AttackData.Damage = 20.f;
	AttackData.DType = EDamageType::EDT_Launch;
	
	return AttackData;
}
FComboAttackData AAISwordManCharacter::GetRangeComboAttackData() const
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
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Launch;
		break;
	case 1:
		AttackData.SectionName = FName("Attack2");
		AttackData.Damage = 12.f;
		AttackData.DType = EDamageType::EDT_Launch;
		break;
	case 2:
		AttackData.SectionName = FName("Attack3");
		AttackData.Damage = 15.f;
		AttackData.DType = EDamageType::EDT_Launch;
		break;
	default:
		AttackData.SectionName = FName("Attack1");
		AttackData.Damage = 10.f;
		AttackData.DType = EDamageType::EDT_Launch;
		break;
	}

	return AttackData;
}