// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSTypes/CSCharacterTypes.h"
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; // 추가
	virtual void Die();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Combat")
	virtual void PlayHitReactMontage();
	virtual void PlayHitReactMontage_Implementation();

	virtual void StopMovement();
	virtual void StopMovement_Implementation();
	
	virtual void Multicast_PlayDeathMontage();
	virtual void Multicast_PlayDeathMontage_Implementation();
	
	virtual void ActivateSuddenDeath();
	virtual bool IsAlive();
	virtual bool IsBlocking();
	virtual bool IsInHitReact();
	bool bIsBlocking = false;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCSAttributeComponent* Attributes;
protected:
	virtual void BeginPlay() override;
	virtual void Attack();
	virtual bool CanAttack();

	UPROPERTY(ReplicatedUsing = OnRep_ActionState, BlueprintReadOnly, Category = "Character State")
	ECharacterTypes ActionState = ECharacterTypes::ECT_Unoccupied; // 기본 상태
	
	UFUNCTION()
	virtual void OnRep_ActionState();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	UAnimMontage* DeathMontage;
	

	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* StimulusSource;
	void SetupStimulusSource();

public:
	UFUNCTION(BlueprintPure, Category = "Character State")
	FORCEINLINE ECharacterTypes GetActionState() const { return ActionState; }
};
