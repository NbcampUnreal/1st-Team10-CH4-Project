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
	UPROPERTY(EditAnywhere, Category = "Input|Fighter|Combo1")
	UInputAction* P_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|Fighter|Combo2")
	UInputAction* K_ComboAction;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* P_AttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* K_AttackMontage;

private:
	// Combo_1(Punch)
	void PlayPCombo1Montage();
	void PlayPCombo2Montage();
	void PlayPCombo3Montage();
	void PlayPComboAnim();

	// Combo_2(Kick)
	void PlayKCombo1Montage();
	void PlayKCombo2Montage();
	void PlayKCombo3Montage();
	void PlayKComboAnim();
};
