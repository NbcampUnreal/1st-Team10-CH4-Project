// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

ACSPlayerController::ACSPlayerController()
{
}

void ACSPlayerController::SetPlayerRole(int PlayerRole)
{
	CharacterRole = PlayerRole;
}

void ACSPlayerController::HealthUpdate(float Health, float MaxHealth)
{
	// TODO:HUD 유효성 체크
	if (IsLocalController())
	{	
		//TODO : PlayerHUD->UpdateHealth(Health, MaxHealth);
	}
}

