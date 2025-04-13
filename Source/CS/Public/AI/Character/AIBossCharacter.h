#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AIBossCharacter.generated.h"


UCLASS()
class CS_API AAIBossCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	AAIBossCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual FComboAttackData GetFirstAttackData() const override;
	virtual FComboAttackData GetSecondAttackData() const override;
	virtual FComboAttackData GetLowComboAttackData() const override;
	virtual FComboAttackData GetRangeComboAttackData() const override;
	
	//소드맨
	UAnimMontage* GetFirstAttackMontage2() const { return FirstAttackMontage2; }
	UAnimMontage* GetSecondAttackMontage2() const { return SecondAttackMontage2; }
	UAnimMontage* GetLowComboAttackMontage2() const { return LowComboAttackMontage2; }
	UAnimMontage* GetRangeComboAttackMontage2() const { return RangeComboAttackMontage2; }
	UAnimMontage* GetBlockMontage2() const { return BlockMontage2; }
	UAnimMontage* GetStunMontageMontage2() const { return StunMontage2; }
	UAnimMontage* GetCrouchAttackMontage2() const { return CrouchAttackMontage2; }
	UAnimMontage* GetSitMontage2() const { return SitMontage2; }
	UAnimMontage* GetJumpRunMontage2() const { return JumpRunMontage2;}
	UAnimMontage* GetDodgeMontage2() const { return DodgeMontage2; }
	
	virtual int FirstAttack_Implementation() override;
	virtual int SecondAttack_Implementation() override;
	virtual int LowComboAttack_Implementation() override;
	virtual int RangeComboAttack_Implementation() override;
	virtual int Block_Implementation() override;
	void HandlePhase2Transition() const;


protected:
	// 스워드맨 모드

	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* FirstAttackMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* SecondAttackMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* BlockMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* StunMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* CrouchAttackMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* LowComboAttackMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* RangeComboAttackMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* JumpRunMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* DodgeMontage2;
	UPROPERTY(EditAnywhere, Category = "Combat2")
	UAnimMontage* SitMontage2;
	
	
	bool bUseSwordManStyle = false;
};
