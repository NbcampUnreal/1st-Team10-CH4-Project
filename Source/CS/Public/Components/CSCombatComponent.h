// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSCombatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CS_API UCSCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCSCombatComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable, Category = "Combat")
    bool GetIsAttacking() const { return bIsAttacking; }

    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetIsAttacking(bool bAttacking);

    UFUNCTION(Server, Reliable)
    void ServerStartAttack();

    UFUNCTION(Server, Reliable)
    void ServerEndAttack();

    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(ReplicatedUsing = OnRep_IsAttacking)
    bool bIsAttacking = false;

    UFUNCTION()
    void OnRep_IsAttacking();
};