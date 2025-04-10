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
	void ServerSpawnProjectile(ACSPlayerCharacter* SpawnPlayer);
	void ServerSpawnProjectile_Implementation(ACSPlayerCharacter* SpawnPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnProjectile(ACSPlayerCharacter* SpawnPlayer);
	void MultiSpawnProjectile_Implementation(ACSPlayerCharacter* SpawnPlayer);

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

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	class UCSAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCSCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AActor> CastProjectile;

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
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComp;
};
