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

