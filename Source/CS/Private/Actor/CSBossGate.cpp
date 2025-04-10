#include "Actor/CSBossGate.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ACSBossGate::ACSBossGate()
{
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	SetRootComponent(DoorMesh);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(DoorMesh);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACSBossGate::OnTriggerBegin);

	OpenOffset = FVector(-200.f, 0.f, 0.f);
	OpenDuration = 1.5f;
	bIsActive = false;
	bHasOpened = false;
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
		const FVector TargetLocation = GetActorLocation() + OpenOffset;

		UKismetSystemLibrary::MoveComponentTo(
			DoorMesh,
			TargetLocation,
			GetActorRotation(),
			false,
			true,
			OpenDuration,
			false,
			EMoveComponentAction::Move,
			FLatentActionInfo()
		);

		bHasOpened = true;
	}
}

