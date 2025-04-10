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


};
