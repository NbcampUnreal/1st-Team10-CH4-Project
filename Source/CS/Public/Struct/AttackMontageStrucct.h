// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackMontageStrucct.generated.h"

/**
 * 
 */
class CS_API AttackMontageStrucct
{
public:
	AttackMontageStrucct();
	~AttackMontageStrucct();
};

USTRUCT(Atomic, BlueprintType)
struct FAttackMontageStruct
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Section;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.f;
};
