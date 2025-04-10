#pragma once

#include "CoreMinimal.h"
#include "AIBossCharacter.h"
#include "AISwordManCharacter.generated.h"

UCLASS()
class CS_API AAISwordManCharacter : public AAIBossCharacter
{
	GENERATED_BODY()

public:
	AAISwordManCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
