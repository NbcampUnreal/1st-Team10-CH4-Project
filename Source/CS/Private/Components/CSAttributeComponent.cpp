// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/CSAttributeComponent.h"

#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CSBaseCharacter.h"
#include "Characters/CSPlayerCharacter.h"
#include "Controller/CSPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

UCSAttributeComponent::UCSAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	//bReplicates = true; 
	SetIsReplicatedByDefault(true);
	

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

void UCSAttributeComponent::ReceiveDamage(float DamageAmount, AController* EventInstigator, AActor* DamageCauser, ELaunchTypes DType, FHitResult HitResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Server: %s - ReceiveDamage Called. Damage: %.1f"), *GetOwner()->GetName(), DamageAmount);

	if (!GetOwner()->HasAuthority()) return;
	if (!IsAlive() || DamageAmount <= 0.f) return;

	ACSBaseCharacter* OwningCharacter = Cast<ACSBaseCharacter>(GetOwner());
	if (!OwningCharacter) return;
	
	if (OwningCharacter->GetStandUpState() == EStandUpType::EST_StandUp) return;
	
	FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(DamageCauser->GetActorLocation(), OwningCharacter->GetActorLocation());
	bool bIsFrontHit = !UKismetMathLibrary::InRange_FloatFloat(
		OwningCharacter->GetActorRotation().Yaw,
		LookRot.Yaw - 90.0f,
		LookRot.Yaw + 90.0f
	);

	if (OwningCharacter->GetActionState() == ECharacterTypes::ECT_Dodge)
	{
		return;
	}

	if (bIsFrontHit)
	{
		if (OwningCharacter->GetActionState() == ECharacterTypes::ECT_Defending)
		{
			FVector LaunchVector = DamageCauser->GetActorForwardVector();
			OwningCharacter->ServerLaunchCharacter(LaunchVector * 250);
			return;
		}
	}
	
	Health = FMath::Clamp(Health - DamageAmount, 0.f, MaxHealth);

	if (!IsAlive())
	{
		HandleDeath();
		return;
	}
	
	if (DType == ELaunchTypes::EDT_Launch)
	{
		CharacterLaunch(DamageCauser);
	}
	else
	{
		OwningCharacter->PlayHitReactMontage();
		
		if (AAIBaseController* AIController = Cast<AAIBaseController>(OwningCharacter->GetController()))
		{
			if (UBlackboardComponent* BB = AIController->GetBlackboardComponent())
			{
				float RandomValue = FMath::FRand();
				if (RandomValue < 0.7f)
				{
					BB->SetValueAsBool("ShouldBlock", true);
					BB->SetValueAsBool("ShouldDodge", false);
				}
				else
				{
					BB->SetValueAsBool("ShouldBlock", false);
					BB->SetValueAsBool("ShouldDodge", true);
				}
			}
		}
		
		FVector LaunchVector = DamageCauser->GetActorForwardVector();
		OwningCharacter->ServerLaunchCharacter(LaunchVector * 500);
	}
}

void UCSAttributeComponent::CharacterLaunch(AActor* DamageCauser)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	if (!IsAlive()) return;

	FVector LaunchVector = DamageCauser->GetActorForwardVector();
	
	if (ACSPlayerCharacter* PlayerCharacter = Cast<ACSPlayerCharacter>(GetOwner()))
	{
		PlayerCharacter->ServerSetActionState(ECharacterTypes::ECT_Launch);
		PlayerCharacter->ServerLaunchCharacter(LaunchVector * 150.f + FVector(0, 0, 500.f));
	}
	
	else if (AAIBaseCharacter* AIC = Cast<AAIBaseCharacter>(GetOwner()))
	{
		AIC->PlayLaunchMontage();
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
