#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSBossGate.generated.h"

UCLASS()
class CS_API ACSBossGate : public AActor
{
	GENERATED_BODY()

public:
	ACSBossGate();

	UFUNCTION(BlueprintCallable)
	void Activate();

protected:
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Door")
	FVector OpenOffset;

	UPROPERTY(EditAnywhere, Category = "Door")
	float OpenDuration;

	bool bIsActive;
	bool bHasOpened;

};

