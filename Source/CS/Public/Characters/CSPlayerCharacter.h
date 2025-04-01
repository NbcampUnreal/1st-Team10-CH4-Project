// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSBaseCharacter.h"
#include "InputActionValue.h"
#include "CSTypes/CSCharacterTypes.h"
#include "CSPlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;

UCLASS()
class CS_API ACSPlayerCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()

public:
	ACSPlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Jump() override;

	void CrouchStart(const FInputActionValue& Value);
	void CrouchEnd(const FInputActionValue& Value);

	/*
	* Callback function for input
	*/

	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* MappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CrouchAction;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterTypes ActionState = ECharacterTypes::ECT_Unoccupied; // 기본 상태

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EFacingDirection FacingDirection = EFacingDirection::EFD_FacingRight; // 기본 방향

	/* Character Components */

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

};
