// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAttributeComponent.h"

UCSAttributeComponent::UCSAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCSAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCSAttributeComponent::ReceiveDamage(float Damage)
{
	Health -= Damage;
	if (!IsAlive())
	{
		// TODO:Call the death function
	}
}

bool UCSAttributeComponent::IsAlive()
{
	return Health > 0;
}

float UCSAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

void UCSAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

