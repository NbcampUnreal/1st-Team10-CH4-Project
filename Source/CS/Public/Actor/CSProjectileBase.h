// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSProjectileBase.generated.h"

class USphereComponent;

UCLASS()
class CS_API ACSProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USceneComponent* SceneComp;
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	USphereComponent* SphereComp;
	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* ProjectileComp;

	FTimerHandle DestroyProjectile;

public:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MultiDestroyProjectile();
	void MultiDestroyProjectile_Implementation();
};
