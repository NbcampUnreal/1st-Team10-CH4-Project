#pragma once

#include "CoreMinimal.h"
#include "AI/PatrolPath.h"
#include "Characters/CSBaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/Interface/CombatInterface.h"
#include "Animation/AnimMontage.h"
#include "CSTypes/CSCharacterTypes.h"
#include "AIBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterStance : uint8
{
	Standing   UMETA(DisplayName = "Standing"),
	Crouching  UMETA(DisplayName = "Crouching"),
	Jumping    UMETA(DisplayName = "Jumping")
};

UCLASS()
class CS_API AAIBaseCharacter : public ACSBaseCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* AIInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stance")
	ECharacterStance CurrentStance = ECharacterStance::Standing;

	UBehaviorTree* GetBehaviorTree() const;
	APatrolPath* GetPatrolPath() const;
	FName GetPunchMontage() const;
	FName GetJumpMontage() const;
	FName GetCrouchMontage() const;
	UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	UAnimMontage* GetBlockMontage() const { return BlockMontage; }
	UAnimMontage* GetJumpAttackMontage() const { return JumpAttackMontage; }
	UAnimMontage* GetCrouchAttackMontage() const { return CrouchAttackMontage; }
	
	virtual int MeleeAttack_Implementation() override;
	void ResumeMovement();
	
	virtual void StopBlock();
	virtual void StopMovement() override;
	
	virtual void Die() override;
	virtual void PlayHitReactMontage() override;


	float LastDamageTime = -100.f;
	void UpdateLastDamageTime();
	bool WasRecentlyDamaged(float DamageTimeout) const;
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	UBehaviorTree* Tree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	APatrolPath* PatrolPath;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* BlockMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* JumpAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* CrouchAttackMontage;
	
	UPROPERTY()
	class UWidgetComponent* WidgetComponenet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSAttributeComponent* AttributeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StunTime", meta = (AllowPrivateAccess = true))
	float HitStunDuration = 0.5f;
	float LastHitTime = 0.f;
	
	FTimerHandle HitReactTimerHandle;
	FTimerHandle BlockTimerHandle;
};