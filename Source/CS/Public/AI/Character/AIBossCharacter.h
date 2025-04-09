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
	
	virtual int Block_Implementation() override;
	virtual int Dodge_Implementation(AActor* Attacker) override;
	virtual int RunAway_Implementation(AActor* Attacker) override;
	
	void Dodge_StartDash(AActor* Attacker);
	void Dodge_MoveToSafeZone(AActor* Attacker);

	UFUNCTION()
	virtual void StopBlock() override;
	
protected:
	virtual void BeginPlay() override;

private:

	FTimerHandle ComboResetTimerHandle;
	int32 CurrentComboIndex = 0;
	
};
