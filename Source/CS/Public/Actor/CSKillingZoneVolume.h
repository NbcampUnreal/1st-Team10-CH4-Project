#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSKillingZoneVolume.generated.h"

UCLASS()
class CS_API ACSKillingZoneVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ACSKillingZoneVolume();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, Category = "KillZone")
	TObjectPtr<class UBoxComponent> BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "KillZone")
	TObjectPtr<class UStaticMeshComponent> VisualBox;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

};
