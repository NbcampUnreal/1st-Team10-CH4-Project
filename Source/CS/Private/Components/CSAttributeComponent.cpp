// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAttributeComponent.h"

#include "AI/Character/AIBaseCharacter.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSPlayerCharacter.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
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

void UCSAttributeComponent::ReceiveDamage(float DamageAmount, AController* EventInstigator, AActor* DamageCauser, EDamageType DType, FHitResult HitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Server: %s - ReceiveDamage Called. Damage: %.1f"), *GetOwner()->GetName(), DamageAmount);
	if (!GetOwner()->HasAuthority()) return;
	if (!IsAlive() || DamageAmount <= 0.f) return;

	ACSBaseCharacter* OwningPlayerCharacter = Cast<ACSBaseCharacter>(GetOwner());
	if (OwningPlayerCharacter)
	{
		// Stand Up Anim -> Not Damaged
		if (OwningPlayerCharacter->GetStandUpState() == EStandUpType::EST_StandUp) return;

		FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(OwningPlayerCharacter->GetActorLocation(), DamageCauser->GetActorLocation());
		bool bLookMinus = UKismetMathLibrary::InRange_FloatFloat(LookRot.Yaw, -180, -90);
		bool bLookPlus = UKismetMathLibrary::InRange_FloatFloat(LookRot.Yaw, 90, 180);

		if (bLookMinus || bLookPlus) // Front
		{
			if (OwningPlayerCharacter->GetActionState() == ECharacterTypes::ECT_Defending) return;
		}
		else // Back(Test)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Back Hit!!!")));
		}
	}

	

	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Server: %s - New Health: %.1f"), *GetOwner()->GetName(), Health);

	if (!IsAlive())
	{
		HandleDeath();
	}

	if (DType == EDamageType::EDT_Launch)
	{
		CharacterLaunch(DamageCauser);
	}
	else
	{
		if (OwningPlayerCharacter)
		{
			FVector LuanchVector = DamageCauser->GetActorForwardVector();
			OwningPlayerCharacter->ServerLaunchCharacter(LuanchVector * 500);
		}
	}
}

void UCSAttributeComponent::CharacterLaunch(AActor* DamageCauser)
{
	if (!GetOwner()->HasAuthority()) return;
	if (!IsAlive()) return;
	
	ACSBaseCharacter* OwningPlayerCharacter = Cast<ACSBaseCharacter>(GetOwner());
	if (OwningPlayerCharacter)
	{
		FVector LuanchVector = DamageCauser->GetActorForwardVector();

		OwningPlayerCharacter->ServerSetActionState(ECharacterTypes::ECT_Launch);
		OwningPlayerCharacter->ServerLaunchCharacter(LuanchVector * 150 + FVector(0, 0, 500));
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
