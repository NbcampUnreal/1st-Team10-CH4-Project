// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CSCombatComponent.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Job")
	EJobTypes JobState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character LayState")
	EGroundTypes GroundState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character StandUp")
	EStandUpType StandUpState;

	UFUNCTION()
	virtual void OnRep_ActionState();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	UAnimMontage* LaunchMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Animation")
	UAnimMontage* GetUpMontage;
	
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* StimulusSource;
	void SetupStimulusSource();

public:
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSetActionState(ECharacterTypes ECTState);
	void ServerSetActionState_Implementation(ECharacterTypes ECTState);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetActionState(ECharacterTypes ECTState);
	void MultiSetActionState_Implementation(ECharacterTypes ECTState);

	UFUNCTION(Server, Reliable)
	void ServerLaunchCharacter(FVector LaunchVector);
	void ServerLaunchCharacter_Implementation(FVector LaunchVector);
	UFUNCTION(NetMulticast, Reliable)
	void MultiLaunchCharacter(FVector LaunchVector);
	void MultiLaunchCharacter_Implementation(FVector LaunchVector);

	UFUNCTION(BlueprintPure, Category = "Character State")
	FORCEINLINE ECharacterTypes GetActionState() const { return ActionState; }
	UFUNCTION(BlueprintPure, Category = "Character State")
	FORCEINLINE EJobTypes GetJobType() const { return JobState; }

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Ground State")
	void ServerSetGroundState(EGroundTypes EGTState);
	void ServerSetGroundState_Implementation(EGroundTypes EGTState);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetGroundState(EGroundTypes EGTState);
	void MultiSetGroundState_Implementation(EGroundTypes EGTState);
	UFUNCTION(BlueprintPure, Category = "Character State")
	FORCEINLINE EGroundTypes GetGroundState() const { return GroundState; }

	UFUNCTION(Server, Reliable, Category = "StandUp State")
	void ServerSetStandUpState(EStandUpType ESTState);
	void ServerSetStandUpState_Implementation(EStandUpType ESTState);
	UFUNCTION(NetMulticast, Reliable)
	void MultiSetStandUpState(EStandUpType ESTState);
	void MultiSetStandUpState_Implementation(EStandUpType ESTState);
	FORCEINLINE EStandUpType GetStandUpState() const { return StandUpState; }

	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(ACSBaseCharacter* SpawnPlayer);
	void ServerSpawnProjectile_Implementation(ACSBaseCharacter* SpawnPlayer);

	UFUNCTION(NetMulticast, Reliable)
	void MultiSpawnProjectile(ACSBaseCharacter* SpawnPlayer);
	void MultiSpawnProjectile_Implementation(ACSBaseCharacter* SpawnPlayer);
	UFUNCTION()
	void PlayLaunchMontage();
	UFUNCTION()
	void PlayGetUpMontage();
	UFUNCTION()
	void OnGetUpMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AActor> CastProjectile;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SceneComp;
};
