#pragma once

#include "CoreMinimal.h"
#include "AI/PatrolPath.h"
#include "AI/DamageType/ComboAttackData.h"
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

	virtual UBehaviorTree* GetBehaviorTree() const;
	virtual APatrolPath* GetPatrolPath() const;

	virtual FComboAttackData GetFirstAttackData() const;
	virtual FComboAttackData GetSecondAttackData() const;
	virtual FComboAttackData GetLowComboAttackData() const;
	virtual FComboAttackData GetRangeComboAttackData() const;

	virtual UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	virtual UAnimMontage* GetFirstAttackMontage() const { return FirstAttackMontage; }
	virtual UAnimMontage* GetSecondAttackMontage() const { return SecondAttackMontage; }
	virtual UAnimMontage* GetLowComboAttackMontage() const { return LowComboAttackMontage; }
	virtual UAnimMontage* GetRangeComboAttackMontage() const { return RangeComboAttackMontage; }
	virtual UAnimMontage* GetBlockMontage() const { return BlockMontage; }
	virtual UAnimMontage* GetStunMontageMontage() const { return StunMontage; }
	virtual UAnimMontage* GetSitMontage() const { return SitMontage; }
	virtual UAnimMontage* GetDodgeMontage() const { return RollingMontage; }

	virtual void PlayHitReactMontage() override;
	
	UFUNCTION()
	virtual int FirstAttack_Implementation() override;
	UFUNCTION()
	virtual int SecondAttack_Implementation() override;
	UFUNCTION()
	virtual int LowComboAttack_Implementation() override;
	UFUNCTION()
	virtual int RangeComboAttack_Implementation() override;
	UFUNCTION()
	virtual int Block_Implementation() override;
	UFUNCTION()
	virtual int Dodge_Implementation(AActor* Attacker) override;
	UFUNCTION()
	virtual int RunAway_Implementation(AActor* Attacker) override;
	UFUNCTION()
	virtual int Rolling_Implementation() override;

	
	UFUNCTION()
	virtual void Dodge_StartDash(AActor* Attacker);
	UFUNCTION()
	virtual void Dodge_MoveToSafeZone(AActor* Attacker);
	UFUNCTION()
	virtual int AI_Attack(UAnimMontage* SelectedMontage, const FComboAttackData& AttackData);


	UFUNCTION()
	virtual void StopBlock();
	UFUNCTION()
	virtual void StopMovement() override;
	UFUNCTION()
	virtual void ResumeMovement();
	UFUNCTION()
	virtual void Die() override;
	UFUNCTION()
	virtual void Landed(const FHitResult& Hit) override;
	UFUNCTION()
	virtual void OnNotify_EndBusy();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	UBehaviorTree* Tree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	APatrolPath* PatrolPath;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", meta = (AllowPrivateAccess = true))
	class UCSCombatComponent* CombatComponent;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* FirstAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* SecondAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* BlockMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* StunMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* LowComboAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* RangeComboAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* RollingMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* SitMontage;
	

	bool bPendingDodgeMove = false;
	UPROPERTY()
	AActor* PendingDodgeAttacker = nullptr;
	
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
	
	UPROPERTY(EditAnywhere, Category = "Phase2|Weapon")
	TSubclassOf<AActor> SwordActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float NomalAIDamage = 8.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float NomalDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float ComboDamage = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float SwoardComboDamage = 15.0f;
};