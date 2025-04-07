#pragma once

#include "CoreMinimal.h"
#include "AI/PatrolPath.h"
#include "Characters/CSBaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/Interface/CombatInterface.h"
#include "Animation/AnimMontage.h"
#include "CSTypes/CSCharacterTypes.h"
#include "AIBaseCharacter.generated.h"


UCLASS()
class CS_API AAIBaseCharacter : public ACSBaseCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* AIInputComponent) override;
	

	UBehaviorTree* GetBehaviorTree() const;
	APatrolPath* GetPatrolPath() const;
	FName GetMontage() const;
	UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	
	UFUNCTION()
	int MeleeAttack_Implementation() override;
	
	virtual void PlayHitReactMontage() override;
	UFUNCTION()
	void ResumeMovement();

	UFUNCTION()
	virtual void StopMovement() override;
	virtual void Die() override;
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	UBehaviorTree* Tree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	APatrolPath* PatrolPath;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;
	
	
	UPROPERTY()
	class UWidgetComponent* WidgetComponenet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSAttributeComponent* AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StunTime", meta = (AllowPrivateAccess = true))
	float HitStunDuration = 0.5f;

	FTimerHandle HitReactTimerHandle;
};