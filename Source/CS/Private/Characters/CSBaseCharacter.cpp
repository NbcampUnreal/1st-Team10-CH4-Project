// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CSBaseCharacter.h"
#include "GameModes/CSGameModeBase.h"
#include "Components/CSAttributeComponent.h"

ACSBaseCharacter::ACSBaseCharacter()
{
 
	PrimaryActorTick.bCanEverTick = true;
}

void ACSBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ACSBaseCharacter::Attack()
{
}

void ACSBaseCharacter::Die()
{
    if (HasAuthority())
    {
        AGameModeBase* GM = GetWorld()->GetAuthGameMode();
        ACSGameModeBase* CurrentGameMode = Cast<ACSGameModeBase>(GM);

        AController* PlayerController = GetController();

        if (CurrentGameMode && PlayerController)
        {
            CurrentGameMode->HandlePlayerDeath(PlayerController);
        }
        
        // Debug lines
        // 
        //if (!CurrentGameMode) UE_LOG(LogTemp, Error, TEXT("Die(): No Current GameMode!"));
        //if (!PlayerController) UE_LOG(LogTemp, Error, TEXT("Die(): Character has no Controller!"));
        
    }
}

bool ACSBaseCharacter::IsAlive()
{
    return Attributes && Attributes->IsAlive();
}

void ACSBaseCharacter::ActivateSuddenDeath()
{
	// TODO: Damage Increase
}

bool ACSBaseCharacter::CanAttack()
{
	return false;
}

void ACSBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

