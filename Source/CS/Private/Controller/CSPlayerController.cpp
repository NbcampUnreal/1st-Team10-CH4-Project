// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"

ACSPlayerController::ACSPlayerController()
{
}

void ACSPlayerController::SetPlayerRole(int PlayerRole)
{
	CharacterRole = PlayerRole;
}

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

