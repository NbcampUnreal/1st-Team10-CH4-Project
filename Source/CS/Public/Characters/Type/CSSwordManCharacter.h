// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CSPlayerCharacter.h"
#include "Struct/AttackMontageStrucct.h"
#include "CSSwordManCharacter.generated.h"

/**
 * 
 */
UCLASS()
class CS_API ACSSwordManCharacter : public ACSPlayerCharacter
{
	GENERATED_BODY()
	
public:
	ACSSwordManCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputMappingContext* SwordManMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputAction* Light_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputAction* Heavy_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputAction* Combination_Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> LightMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> HeavyMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> CombinationMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TObjectPtr<UAnimMontage> CounterMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CounterHoldThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Damage")
	float CounterAttackDamage = 25.f;

protected:
	virtual void BeginPlay() override;

private:
	void HandleLightAttackPress();
	void HandleLightAttackRelease();
	void CheckForCounterAttack();
	void PlayLightComboMontage();
	void PlayHeavyAttackAnim();
	void PlayCombinationAnim();

	FTimerHandle LightAttackHoldTimer;

	bool bIsLightAttackPressed = false;
	bool bPerformedCounter = false;


};
