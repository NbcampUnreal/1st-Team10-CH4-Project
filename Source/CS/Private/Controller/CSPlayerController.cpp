// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/CSPlayerController.h"

void ACSPlayerController::SetPlayerRole(int PlayerRole)
{
	CharacterRole = PlayerRole;
}

void ACSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Player %d has joined!"), CharacterRole);
}

