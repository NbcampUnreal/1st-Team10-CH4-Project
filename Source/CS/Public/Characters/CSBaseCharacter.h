// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSBaseCharacter.generated.h"

class UCSAttributeComponent;

UCLASS()
class CS_API ACSBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACSBaseCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Die();
	
	virtual void PlayHitReactMontage();
	virtual void PlayHitReactMontage_Implementation();

	virtual void StopMovement();
	virtual void StopMovement_Implementation();

protected:
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual bool CanAttack();
	bool IsAlive();

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void ActivateSuddenDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCSAttributeComponent* Attributes;

};
