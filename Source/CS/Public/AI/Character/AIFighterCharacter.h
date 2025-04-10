#pragma once

#include "CoreMinimal.h"
#include "AIBossCharacter.h"
#include "AIFighterCharacter.generated.h"

UCLASS()
class CS_API AAIFighterCharacter : public AAIBossCharacter
{
	GENERATED_BODY()

public:
	AAIFighterCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
