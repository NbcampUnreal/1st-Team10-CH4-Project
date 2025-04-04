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

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Porjectile|Control")
	float speed;

	FTimerHandle DestroyProjectile;

public:	
	UFUNCTION(Server, Reliable)
	void ServerDestroyProjectile();

	UFUNCTION(NetMulticast, Reliable)
	void MultiDestoryProjectile();
};
