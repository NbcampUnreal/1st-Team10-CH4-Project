// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAttributeComponent.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSPlayerCharacter.h"
#include "Controller/CSPlayerController.h"
#include "Net/UnrealNetwork.h"

UCSAttributeComponent::UCSAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicated(true);

	MaxHealth = 100.f;
	Health = MaxHealth;
}

void UCSAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;

	ACharacter* Owner = Cast<ACharacter>(GetOwner());
	if (Owner)
	{
		ACSPlayerController* PlayerController = Cast<ACSPlayerController>(Owner->GetController());
		if (PlayerController && PlayerController->IsLocalController())
		{
			OwningPlayerController = PlayerController;
		}
	}
}


void UCSAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCSAttributeComponent, Health);
	DOREPLIFETIME(UCSAttributeComponent, MaxHealth);
}

void UCSAttributeComponent::ReceiveDamage(float DamageAmount, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Server: %s - ReceiveDamage Called. Damage: %.1f"), *GetOwner()->GetName(), DamageAmount);
	if (!GetOwner()->HasAuthority()) return;
	if (!IsAlive() || DamageAmount <= 0.f) return;

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Server: %s - New Health: %.1f"), *GetOwner()->GetName(), Health); // 변경된 체력 로그
	if (!IsAlive())
	{
		HandleDeath();
	}
}

void UCSAttributeComponent::HandleDeath()
{
	ACSBaseCharacter* OwningPlayerCharacter = Cast<ACSBaseCharacter>(GetOwner());
	if (OwningPlayerCharacter)
	{
		OwningPlayerCharacter->Die();
	}
}
bool UCSAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UCSAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCSAttributeComponent::OnRep_Health()
{
	UE_LOG(LogTemp, Warning, TEXT("Client: %s - OnRep_Health Called. Current Health: %.1f"), *GetOwner()->GetName(), Health); // OnRep 호출 및 현재 체력 로그

	ACSPlayerCharacter* PlayerCharacter = Cast<ACSPlayerCharacter>(GetOwner());

	if (PlayerCharacter)
	{
		ACSPlayerController* PlayerController = Cast<ACSPlayerController>(PlayerCharacter->GetController());

		if (PlayerController)
		{
			PlayerController->HealthUpdate(Health, MaxHealth);
		}
	}
}
