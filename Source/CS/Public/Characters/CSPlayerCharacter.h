// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CSBaseCharacter.h"
#include "InputActionValue.h"
#include "CSPlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;
class USkeletalMeshComponent;
class UAnimMontage;

UCLASS()
class CS_API ACSPlayerCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()

public:
	ACSPlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Jump() override;

	void CrouchStart(const FInputActionValue& Value);
	void CrouchEnd(const FInputActionValue& Value);
 	void PlayPlayerMontage(UAnimMontage* PlayMontage, FName Section);
	void GuardStart();
	void GuardEnd();
	void DodgeStart();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DodgeEnd();
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartAttack(UAnimMontage* PlayMontage, FName Section);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DuringAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndAttack();

	UFUNCTION(BlueprintCallable)
	void ComboStateChange();

	UFUNCTION(NetMulticast, Reliable)
	virtual void StopMovement() override;
	virtual void StopMovement_Implementation() override;
	
	UFUNCTION(Server, Reliable)
	void Server_PerformDodge();
	void Server_PerformDodge_Implementation();

	UFUNCTION(Server, Reliable)
	void Server_FinishDodge();
	void Server_FinishDodge_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDodgeMontage();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateSuddenDeathMode();

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* GuardAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

	//UPROPERTY(EditAnywhere, Category = "Combat")
	//UAnimMontage* DodgeMontage;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCSCombatComponent* CombatComponent;
	
	// Montage animation server connect function "DuringAttack" Check
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ComboCheck();

private:
	void UpdateFacingDirection(float XInput);
	void UpdateRotation();

	UPROPERTY(ReplicatedUsing = OnRep_FacingDirection)
	EFacingDirection FacingDirection = EFacingDirection::EFD_FacingRight;

	UFUNCTION(Server, Reliable)
	void ServerSetFacingDirection(EFacingDirection NewDirection);

	UFUNCTION()
	void OnRep_FacingDirection();

	/* Character Components */

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;
};
