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
	UInputAction* Lignt_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputAction* K_ComboAction;
	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputAction* Lignt_Casting;
	UPROPERTY(EditAnywhere, Category = "Input|SwordMan")
	UInputAction* K_Casting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> LightMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TArray<FAttackMontageStruct> LightCastMontage;

private:
	// Combo_1(Light)
	void PlayLightComboMontage();
	void PlayLightComboAnim();

	// Casting
	void PlayLightCastAnim();
};
