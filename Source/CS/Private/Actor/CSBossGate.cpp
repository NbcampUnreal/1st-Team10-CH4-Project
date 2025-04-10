#include "Actor/CSBossGate.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ACSBossGate::ACSBossGate()
{
	PrimaryActorTick.bCanEverTick = true;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	SetRootComponent(DoorMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(DoorMesh);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACSBossGate::OnTriggerBegin);

	OpenOffset = FVector(-200.f, 0.f, 0.f);
	OpenDuration = 1.5f;
	OriginalLocation = FVector::ZeroVector;
	TargetLocation = FVector::ZeroVector;
	ElapsedOpenTime = 0.0f;
	bIsActive = false;
	bHasOpened = false;
}

void ACSBossGate::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasOpened && ElapsedOpenTime < OpenDuration)
	{
		ElapsedOpenTime += DeltaTime;

		const float Alpha = FMath::Clamp(ElapsedOpenTime / OpenDuration, 0.f, 1.f);
		const FVector NewLocation = FMath::Lerp(OriginalLocation, TargetLocation, Alpha);

		SetActorLocation(NewLocation);

		if (Alpha >= 1.f)
		{
			OnDoorOpened();
		}
	}
}

void ACSBossGate::Activate()
{
	bIsActive = true;
}

void ACSBossGate::OnTriggerBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bIsActive || bHasOpened) return;

	if (OtherActor && OtherActor->ActorHasTag("SinglePlayer"))
	{
		OriginalLocation = GetActorLocation();
		TargetLocation = OriginalLocation + OpenOffset;

		ElapsedOpenTime = 0.0f;
		bHasOpened = true;
	}
}

void ACSBossGate::OnDoorOpened()
{
	Destroy();
}

