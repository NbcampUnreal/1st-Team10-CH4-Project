#include "AI/Character/AIFighterCharacter.h"


AAIFighterCharacter::AAIFighterCharacter()
{
	
	PrimaryActorTick.bCanEverTick = true;
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

