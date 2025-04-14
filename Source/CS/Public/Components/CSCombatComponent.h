// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSTypes/CSCharacterTypes.h"
#include "Struct/AttackMontageStrucct.h"
#include "CSCombatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CS_API UCSCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCSCombatComponent();

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool GetIsAttacking() const { return bIsAttacking; }
    bool GetCanCombo();

	UFUNCTION(BlueprintPure, Category = "Combat")
    float GetCurrentAttackDamage() const;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetIsAttacking(bool bAttacking);

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetCurrentAttackDamage(float Damage);

    UFUNCTION(NetMulticast, Reliable)
    void MultiSetMontageData(UAnimMontage* PlayMontage, FName Section);
    void MultiSetMontageData_Implementation(UAnimMontage* PlayMontage, FName Section);

    UFUNCTION(Server, Reliable)
    void ServerSetMontageData(UAnimMontage* PlayMontage, FName Section);
    void ServerSetMontageData_Implementation(UAnimMontage* PlayMontage, FName Section);

    UFUNCTION(Server, Reliable)
    void ServerStartAttack();
    void ServerStartAttack_Implementation();

    UFUNCTION(Server, Reliable)
    void ServerEndAttack();
    void ServerEndAttack_Implementation();

    // Combo Function
    void Combo1CntIncrease();
    int32 GetCombo1Cnt();
    void Combo2CntIncrease();
    int32 GetCombo2Cnt();
    void CanComboChange(bool Check);

    UFUNCTION(BlueprintCallable, Category = "Combo")
    void ResetComboData();

	UFUNCTION(BlueprintCallable, Category = "Combat")
    void ClearHitActors();

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void Server_PerformHitCheck(FName TraceStartName, FName TraceEndName, float AttackDamage, ELaunchTypes DType);
	void Server_PerformHitCheck_Implementation(FName TraceStartName, FName TraceEndName, float AttackDamage, ELaunchTypes DType);

    UFUNCTION()
    void PerformAttack(UAnimMontage* Montage, FName SectionName);


    UFUNCTION(BlueprintCallable)
    bool IsPendingHit() const { return bPendingHit; }
    UFUNCTION(BlueprintCallable)
    FName GetPendingTraceStart() const { return PendingTraceStart; }
    UFUNCTION(BlueprintCallable)
    FName GetPendingTraceEnd() const { return PendingTraceEnd; }
    UFUNCTION(BlueprintCallable)
    float GetPendingDamage() const { return PendingDamage; }
    UFUNCTION(BlueprintCallable)
    ELaunchTypes GetPendingDType() const { return PendingDType; }
    UFUNCTION(BlueprintCallable)
    void ClearPendingHit() { bPendingHit = false; }
    UFUNCTION(BlueprintCallable)
    void SetPendingHit(FName InTraceStart, FName InTraceEnd, float InDamage, ELaunchTypes InDType);
protected:
    virtual void BeginPlay() override;

    UPROPERTY(ReplicatedUsing = OnRep_IsAttacking)
    bool bIsAttacking = false;

    UFUNCTION()
    void OnRep_IsAttacking();
    UPROPERTY()
    UAnimMontage* ServerPlayMontage;
    FName ServerSection;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> HitActorsThisAttack;

    float CurrentAttackDamage;

    // Combo Data
    int32 iCombo_1_Cnt;
    int32 iCombo_2_Cnt;
    bool bCanCombo;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Combat")
    bool bIsSuddenDeathActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    float SuddenDeathDamage;
    
    
    bool bPendingHit;
    FName PendingTraceStart;
    FName PendingTraceEnd;
    float PendingDamage;
    ELaunchTypes PendingDType;
private:

    UFUNCTION(Server, Reliable, WithValidation)
    void ServerSetIsAttacking(bool bAttacking);
    void ServerSetIsAttacking_Implementation(bool bAttacking);
    bool ServerSetIsAttacking_Validate(bool bAttacking);

public:
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetSuddenDeathActive(bool bActive);

    FORCEINLINE bool IsSuddenDeathActive() const { return bIsSuddenDeathActive; }
    
};