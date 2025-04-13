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

