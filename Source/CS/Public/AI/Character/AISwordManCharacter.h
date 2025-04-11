#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AISwordManCharacter.generated.h"

UCLASS()
class CS_API AAISwordManCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	AAISwordManCharacter();

	virtual FComboAttackData GetFirstAttackData() const override;
	virtual FComboAttackData GetSecondAttackData() const override;
	virtual FComboAttackData GetLowComboAttackData() const override;
	virtual FComboAttackData GetRangeComboAttackData() const override;
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
