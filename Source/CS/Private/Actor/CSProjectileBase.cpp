// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/CSProjectileBase.h"
#include "Components/SphereComponent.h"

// Sets default values
ACSProjectileBase::ACSProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphrerComponent"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->OnComponentHit.AddDynamic(this, &ACSProjectileBase::OnHit);
}

// Called when the game starts or when spawned
void ACSProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimer(
		DestroyProjectile, this, &ACSProjectileBase::ServerDestroyProjectile, 5.0f, false
	);
}

void ACSProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Damage Event
	AActor* SpawnActor = GetOwner();

	ServerDestroyProjectile();
}

void ACSProjectileBase::ServerDestroyProjectile_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(DestroyProjectile);
	MultiDestoryProjectile();
}

void ACSProjectileBase::MultiDestoryProjectile_Implementation()
{
	Destroy();
}


