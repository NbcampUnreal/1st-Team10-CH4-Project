#include "AI/Character/AIFighterCharacter.h"

#include "AI/Controller/AIFighterController.h"


AAIFighterCharacter::AAIFighterCharacter()
{
	AIControllerClass = AAIFighterController::StaticClass();
	Tags.Add(FName("Fighter"));
}


void AAIFighterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


void AAIFighterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAIFighterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

int AAIFighterCharacter::RangeComboAttack_Implementation()
{
	FComboAttackData AttackData = GetRangeComboAttackData();
	FName tracestart;
	FName traceend;
	
	tracestart = "RightFistSocket";
	traceend = "RightFistSocketEnd";
	AI_Attack(GetRangeComboAttackMontage(), AttackData, tracestart, traceend);
		
	
	return 1;
}
