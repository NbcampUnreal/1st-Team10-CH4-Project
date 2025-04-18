// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSPlayerCharacter.h"
#include "Struct/AttackMontageStrucct.h"
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
	UInputAction* P_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|Fighter")
	UInputAction* K_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|Fighter")
	UInputAction* P_Casting;
	UPROPERTY(EditAnywhere, Category = "Input|Fighter")
	UInputAction* K_Casting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> PunchMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> KickMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> PunchCastMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> KickCastMontage;

private:
	// Combo_1(Punch)
	void PlayPComboMontage();
	void PlayPComboAnim();

	// Combo_2(Kick)
	void PlayKComboMontage();
	void PlayKComboAnim();

	// Casting
	void PlayPCastingAnim();
	void PlayKCastingAnim();
};
