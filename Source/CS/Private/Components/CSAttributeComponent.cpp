// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAttributeComponent.h"
#include "Characters/CSPlayerCharacter.h"
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
	DOREPLIFETIME(UCSAttributeComponent, FacingDirection);
}

bool UCSAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

float UCSAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

void UCSAttributeComponent::ServerUpdateFacingDirection_Implementation(float XInput)
{
    UpdateFacingDirection(XInput);
}

void UCSAttributeComponent::UpdateFacingDirection(float XInput)
{
    if (GetOwner()->HasAuthority())
    {
        if (XInput > 0.f)
        {
            FacingDirection = EFacingDirection::EFD_FacingRight;
        }
        else if (XInput < 0.f)
        {
            FacingDirection = EFacingDirection::EFD_FacingLeft;
        }
    }
    else
    {
        ServerUpdateFacingDirection(XInput);
    }
}

void UCSAttributeComponent::ServerUpdateRotation_Implementation(FRotator NewRotation)
{
    ACSPlayerCharacter* PlayerCharacter = Cast<ACSPlayerCharacter>(GetOwner());
    if (PlayerCharacter)
    {
        PlayerCharacter->SetActorRotation(NewRotation);
    }
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

void UCSAttributeComponent::OnRep_FacingDirection()
{
    ACSPlayerCharacter* PlayerCharacter = Cast<ACSPlayerCharacter>(GetOwner());
    if (PlayerCharacter)
    {
        float TargetYaw = (FacingDirection == EFacingDirection::EFD_FacingRight) ? -90.f : 90.f;
        FRotator NewRotation = FRotator(0.f, TargetYaw, 0.f);

        if (GetOwner()->HasAuthority())
        {
            PlayerCharacter->SetActorRotation(NewRotation);
        }
        else
        {
            ServerUpdateRotation(NewRotation);
        }
    }
}
