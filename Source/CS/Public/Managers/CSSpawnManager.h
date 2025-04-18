
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSTypes/CSGameTypes.h"
#include "CSSpawnManager.generated.h"

UCLASS()
class CS_API ACSSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSSpawnManager();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	ESpawnSlotType SlotType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UCapsuleComponent> CapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UArrowComponent> ArrowComponent;
};
