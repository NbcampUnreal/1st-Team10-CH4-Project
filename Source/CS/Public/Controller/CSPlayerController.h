// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CSPlayerController.generated.h"

UCLASS()
class CS_API ACSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACSPlayerController();
	void SetPlayerRole(int PlayerRole);
	void HealthUpdate(float Health, float MaxHealth);
protected:
	virtual void BeginPlay() override;

private:
	int32 CharacterRole;

	//UPROPERTY()
	//class UHUDWidget* PlayerHUD;
	
};
