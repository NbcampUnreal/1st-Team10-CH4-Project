#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AIFighterCharacter.generated.h"

UCLASS()
class CS_API AAIFighterCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	AAIFighterCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual int RangeComboAttack_Implementation() override;
	
};
