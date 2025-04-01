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
	void SetPlayerRole(int PlayerRole);

protected:
	virtual void BeginPlay() override;

private:
	int32 CharacterRole;
	
};
