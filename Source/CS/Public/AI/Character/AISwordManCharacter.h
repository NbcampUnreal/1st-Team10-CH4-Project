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
	
	FName GetfirstAttackName() const;
	FName GetsecondAttackName() const;
	FName GetLowComboAttackName() const;
	FName GetRangeComboAttackName() const;
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
