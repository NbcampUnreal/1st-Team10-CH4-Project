// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CSBaseCharacter.h"

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
}

bool ACSBaseCharacter::CanAttack()
{
	return false;
}

bool ACSBaseCharacter::IsAlive()
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

