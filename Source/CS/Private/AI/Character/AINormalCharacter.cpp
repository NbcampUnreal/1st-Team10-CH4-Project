#include "AI/Character/AINormalCharacter.h"
#include "AI/Controller/AINormalController.h"
#include "Components/CSCombatComponent.h"

AAINormalCharacter::AAINormalCharacter()
{
	AIControllerClass = AAINormalController::StaticClass();
	Tags.Add(FName("Normal"));
}

void AAINormalCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AAINormalCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAINormalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

FComboAttackData AAINormalCharacter::GetFirstAttackData() const
{
	FComboAttackData AttackData;

	const int32 RandomIndex = FMath::RandRange(0, 2);

	switch (RandomIndex)
	{
	case 0:
		AttackData.SectionName = FName("Punch1");
		AttackData.Damage = 10.f;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	case 1:
		AttackData.SectionName = FName("Punch2");
		AttackData.Damage = 10.f;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	case 2:
		AttackData.SectionName = FName("Punch3");
		AttackData.Damage = 10.f;
		AttackData.DType = ELaunchTypes::EDT_Nomal;
		break;
	}

	return AttackData;
}
