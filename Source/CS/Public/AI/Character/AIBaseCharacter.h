#pragma once

#include "CoreMinimal.h"
#include "AI/PatrolPath.h"
#include "Characters/CSBaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIBaseCharacter.generated.h"

UCLASS()
class CS_API AAIBaseCharacter : public ACSBaseCharacter
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* AIInputComponent) override;
	UBehaviorTree* GetBehaviorTree() const;
	APatrolPath* GetPatrolPath() const;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI",meta=(AllowPrivateAccess=true))
	UBehaviorTree* Tree;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI",meta=(AllowPrivateAccess=true))
	APatrolPath* PatrolPath;
	
};
