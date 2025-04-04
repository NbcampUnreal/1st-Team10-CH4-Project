#pragma once

#include "CoreMinimal.h"
#include "AI/PatrolPath.h"
#include "Characters/CSBaseCharacter.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AI/Interface/CombatInterface.h"
#include "Animation/AnimMontage.h"
#include "AIBaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	FirstAttack     UMETA(DisplayName = "1. Attack"),
	SecondAttack     UMETA(DisplayName = "2. Attack"),
	ThirdAttack     UMETA(DisplayName = "3. Attack"),
	FourthAttack     UMETA(DisplayName = "4. Attack"),
	FifthAttack     UMETA(DisplayName = "5. Attack"),
};

UCLASS()
class CS_API AAIBaseCharacter : public ACSBaseCharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* AIInputComponent) override;
	
	UBehaviorTree* GetBehaviorTree() const;
	APatrolPath* GetPatrolPath() const;
	UAnimMontage* GetMontage() const;

	int MeleeAttack_Implementation() override;
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI",meta=(AllowPrivateAccess=true))
	UBehaviorTree* Tree;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="AI",meta=(AllowPrivateAccess=true))
	APatrolPath* PatrolPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat", meta=(AllowPrivateAccess=true))
	TArray<UAnimMontage*> AttackMontages;
};
