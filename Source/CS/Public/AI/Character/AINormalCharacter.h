#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "AINormalCharacter.generated.h"

UCLASS()
class CS_API AAINormalCharacter : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	AAINormalCharacter();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual FComboAttackData GetFirstAttackData() const override;
};
