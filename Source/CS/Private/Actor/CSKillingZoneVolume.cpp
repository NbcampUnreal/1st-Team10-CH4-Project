#include "Actor/CSKillingZoneVolume.h"
#include "Characters/CSBaseCharacter.h"
#include "Components/BoxComponent.h"

ACSKillingZoneVolume::ACSKillingZoneVolume()
{
	bReplicates = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("KillZone"));
	SetRootComponent(BoxComponent);

	BoxComponent->ShapeColor = FColor::Red;
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACSKillingZoneVolume::OnOverlapBegin);

	VisualBox = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualBox"));
	VisualBox->SetupAttachment(BoxComponent);
	VisualBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VisualBox->SetHiddenInGame(true);
	VisualBox->bIsEditorOnly = true;
}

void ACSKillingZoneVolume::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		SetActorEnableCollision(false);
	}
}

void ACSKillingZoneVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (BoxComponent && VisualBox)
	{
		FVector Extent = BoxComponent->GetUnscaledBoxExtent();
		FVector MeshScale = Extent / 50.f; 
		VisualBox->SetRelativeScale3D(MeshScale);
	}
}

void ACSKillingZoneVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority() || !OtherActor) return;
	
	if (ACSBaseCharacter* CSBaseCharacter = Cast<ACSBaseCharacter>(OtherActor))
	{
		CSBaseCharacter->Die();
	}
}

