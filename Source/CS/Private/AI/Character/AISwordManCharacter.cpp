#include "AI/Character/AISwordManCharacter.h"

#include "AI/Controller/AISwordManController.h"
#include "Kismet/GameplayStatics.h"

AAISwordManCharacter::AAISwordManCharacter()
{
	AIControllerClass = AAISwordManController::StaticClass();
	Tags.Add(FName("SwordMan"));
}


void AAISwordManCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!PatrolPath)
	{
		TArray<AActor*> FoundPaths;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APatrolPath::StaticClass(), FoundPaths);

		for (AActor* Actor : FoundPaths)
		{
			if (Actor->ActorHasTag(FName("CoopPatrol")))
			{
				PatrolPath = Cast<APatrolPath>(Actor);
				break;
			}
		}
	}
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
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	case 1:
		AttackData.SectionName = FName("Light2");
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	default:
		AttackData.SectionName = FName("Light1");
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	}
	AttackData.Damage = NomalDamage;
	AttackData.DType = ELaunchTypes::EDT_Nomal;
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
	AttackData.Damage = NomalDamage;
	AttackData.DType = ELaunchTypes::EDT_Nomal;
	return AttackData;
}

FComboAttackData AAISwordManCharacter::GetRangeComboAttackData() const
{
	FComboAttackData AttackData;

	AttackData.SectionName = FName("Counter");
	AttackData.TraceStart = FName("hand_r_socket");
	AttackData.TraceEnd = FName("hand_r_trace");
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
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	case 1:
		AttackData.SectionName = FName("Attack2");
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	case 2:
		AttackData.SectionName = FName("Attack3");
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	default:
		AttackData.SectionName = FName("Attack1");
		AttackData.TraceStart = FName("hand_r_socket");
		AttackData.TraceEnd = FName("hand_r_trace");
		break;
	}
	AttackData.Damage = SwoardComboDamage;
	AttackData.DType = ELaunchTypes::EDT_Launch;
	return AttackData;
}
