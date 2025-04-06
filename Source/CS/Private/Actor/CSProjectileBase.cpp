// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/CSProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACSProjectileBase::ACSProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneComp);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphrerComponent"));
	SphereComp->SetupAttachment(RootComponent);

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileComp->InitialSpeed = 800;
	ProjectileComp->MaxSpeed = 800;
	ProjectileComp->ProjectileGravityScale = 0;
}

// Called when the game starts or when spawned
void ACSProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACSProjectileBase::OnOverlapBegin);

	GetWorld()->GetTimerManager().SetTimer(
		DestroyProjectile, this, &ACSProjectileBase::ServerDestroyProjectile, 5.0f, false
	);
}

void ACSProjectileBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Damage Event
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Overlap")));
	AActor* SpawnActor = GetOwner();

	if (SpawnActor != OtherActor)
	{
		ServerDestroyProjectile();
	}
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


