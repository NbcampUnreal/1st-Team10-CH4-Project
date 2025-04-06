// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSAnimInstance.generated.h"



class ACSBaseCharacter;
/**
 * 
 */
UCLASS()
class CS_API UCSAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class ACSPlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
	bool bIsAlive = true;

	UPROPERTY(BlueprintReadOnly, Category = "Character State")
	ECharacterTypes CurrentActionState = ECharacterTypes::ECT_Unoccupied;

	UPROPERTY()
	TWeakObjectPtr<ACSBaseCharacter> BaseCharacter;
};
