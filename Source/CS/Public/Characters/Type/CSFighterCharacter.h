// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSPlayerCharacter.h"
#include "CSFighterCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CS_API ACSFighterCharacter : public ACSPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ACSFighterCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Input|Fighter")
	UInputMappingContext* FighterMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input|Fighter")
	UInputAction* HCombo1Action;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* HAttackMontage;

private:
	void PlayHCombo1Montage();
};
