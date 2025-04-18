#include "Managers/CSSpawnManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"

ACSSpawnManager::ACSSpawnManager()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComponent);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->SetupAttachment(CapsuleComponent);

	SlotType = ESpawnSlotType::None;
}


