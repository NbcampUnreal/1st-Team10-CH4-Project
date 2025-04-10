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

	virtual FName GetfirstAttackName() const;
	virtual FName GetsecondAttackName() const;
	virtual FName GetLowComboAttackName() const;
	virtual FName GetRangeComboAttackName() const;
	FName GetJumpName() const;
	FName GetCrouchName() const;
	
	UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	UAnimMontage* GetfirstAttackMontage() const { return firstAttackMontage; }
	UAnimMontage* GetsecondAttackMontage() const { return secondAttackMontage; }
	UAnimMontage* GetLowComboAttackMontage() const { return LowComboAttackMontage; }
	UAnimMontage* GetRangeComboAttackMontage() const { return RangeComboAttackMontage; }
	UAnimMontage* GetBlockMontage() const { return BlockMontage; }
	UAnimMontage* GetJumpAttackMontage() const { return JumpAttackMontage; }
	UAnimMontage* GetCrouchAttackMontage() const { return CrouchAttackMontage; }
	UAnimMontage* GetSitMontage() const { return SitMontage; }
	UAnimMontage* GetJumpRunMontage() const { return JumpRunMontage;}
	UAnimMontage* GetDodgeMontage() const { return DodgeMontage; }
	
	virtual int firstAttack_Implementation() override;
	virtual int secondAttack_Implementation() override;
	virtual int LowComboAttack_Implementation() override;
	virtual int RangeComboAttack_Implementation() override;
	virtual int Block_Implementation() override;
	virtual int Dodge_Implementation(AActor* Attacker) override;
	virtual int RunAway_Implementation(AActor* Attacker) override;
	virtual void Dodge_StartDash(AActor* Attacker);
	virtual void Dodge_MoveToSafeZone(AActor* Attacker);

	virtual int AI_Attack(UAnimMontage* SelectedMontage, FName SectionName);
	
	virtual void StopBlock();
	virtual void StopMovement() override;
	virtual void ResumeMovement();
	
	virtual void Die() override;
	virtual void PlayHitReactMontage() override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	UBehaviorTree* Tree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	APatrolPath* PatrolPath;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, Category = "UI")
	class UWidgetComponent* WidgetComponenet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSAttributeComponent* AttributeComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSCombatComponent* CombatComponent;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* firstAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* secondAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* BlockMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* JumpAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* CrouchAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* LowComboAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* RangeComboAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* JumpRunMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* SitMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AActor> CastProjectile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StunTime", meta = (AllowPrivateAccess = true))
	float HitStunDuration = 0.5f;
	float LastHitTime = 0.f;

	mutable int32 CurrentPunchIndex = 0;
	mutable float LastPunchTime = 0.0f;
	
	mutable int32 CurrentKickIndex = 0;
	mutable float LastKickTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboTiomer", meta = (AllowPrivateAccess = true))
	float ComboResetCooldown = 1.0f;
	
	int32 CurrentComboIndex = 0;
	
	FTimerHandle ComboResetTimerHandle;
	FTimerHandle HitReactTimerHandle;
	FTimerHandle BlockTimerHandle;
};