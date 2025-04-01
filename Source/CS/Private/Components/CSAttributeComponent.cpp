// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAttributeComponent.h"
#include "Net/UnrealNetwork.h"

UCSAttributeComponent::UCSAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
}

void UCSAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCSAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCSAttributeComponent, Health);
	DOREPLIFETIME(UCSAttributeComponent, MaxHealth);
}

bool UCSAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

float UCSAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

void UCSAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UCSAttributeComponent::OnRep_Health()
{
	UE_LOG(LogTemp, Log, TEXT("Health Replicated: %f"), Health);
}

void UCSAttributeComponent::OnRep_MaxHealth()
{
	UE_LOG(LogTemp, Log, TEXT("MaxHealth Replicated: %f"), MaxHealth);
}
