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

FName AAISwordManCharacter::GetfirstAttackName() const
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

	switch (CurrentPunchIndex)
	{
	case 0: return FName("Light1");
	case 1: return FName("Light2");
	
	}
	
	return FName("Light1");
}

FName AAISwordManCharacter::GetsecondAttackName() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;

	if (CurrentTime - LastKickTime > ComboResetCooldown)
	{
		CurrentKickIndex = 0;
	}
	else
	{
		CurrentKickIndex = (CurrentKickIndex + 1) % 2;
	}

	LastKickTime = CurrentTime;

	switch (CurrentKickIndex)
	{
	case 0: return FName("Attack1");
	case 1: return FName("Attack2");
	}

	return FName("Attack1");
}

FName AAISwordManCharacter::GetLowComboAttackName() const
{
	return FName("Counter");
}
FName AAISwordManCharacter::GetRangeComboAttackName() const
{
	const float CurrentTime = GetWorld()->TimeSeconds;

	if (CurrentTime - LastKickTime > ComboResetCooldown)
	{
		CurrentKickIndex = 0;
	}
	else
	{
		CurrentKickIndex = (CurrentKickIndex + 1) % 3;
	}

	LastKickTime = CurrentTime;

	switch (CurrentKickIndex)
	{
	case 0: return FName("Attack1");
	case 1: return FName("Attack2");
	case 2: return FName("Attack3");
	}

	return FName("Attack1");
}