
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class CS_API APatrolPath : public AActor
{
	GENERATED_BODY()

public:
	APatrolPath();

	FVector GetPatrolPoint(int const index) const;
	int Num() const;
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (MakeEditWidget="true", AllowPrivateAccess = "true"))
	TArray<FVector> PatrolPoints;

	
};