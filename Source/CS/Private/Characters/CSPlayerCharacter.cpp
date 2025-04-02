// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CSPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CSAnimInstance.h"
#include "Components/CSAttributeComponent.h"
#include "Components/CSCombatComponent.h"
#include "Net/UnrealNetwork.h"

ACSPlayerCharacter::ACSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->Crouch(true);
	SetReplicateMovement(true);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = false;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	AttributeComponent = CreateDefaultSubobject<UCSAttributeComponent>(TEXT("AttributeComponent"));
	CombatComponent = CreateDefaultSubobject<UCSCombatComponent>(TEXT("CombatComponent"));
}

void ACSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void ACSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ACSPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACSPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ACSPlayerCharacter::CrouchStart);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACSPlayerCharacter::CrouchEnd);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACSPlayerCharacter::PlayAttackMontage);
	}
}

void ACSPlayerCharacter::Jump()
{
	if (ActionState == ECharacterTypes::ECT_Unoccupied)
	{
		Super::Jump();
	}
}

void ACSPlayerCharacter::CrouchStart(const FInputActionValue& Value)
{
	if (CanCrouch())
	{
		Crouch();
	}
}

void ACSPlayerCharacter::CrouchEnd(const FInputActionValue& Value)
{
	UnCrouch();
}


void ACSPlayerCharacter::Move(const FInputActionValue& Value)
{

	if (ActionState != ECharacterTypes::ECT_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FVector ForwardDirection = FVector(0.f, -1.f, 0.f); // Y축 방향
	AddMovementInput(ForwardDirection, MovementVector.X);

	if (AttributeComponent)
	{
		AttributeComponent->UpdateFacingDirection(MovementVector.X);
		EFacingDirection Facing = AttributeComponent->GetFacingDirection();
		float TargetYaw = (Facing == EFacingDirection::EFD_FacingRight) ? -90.f : 90.f;
		FRotator NewRotation = FRotator(0.f, TargetYaw, 0.f);
		SetActorRotation(NewRotation);
	}
}

void ACSPlayerCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		FName SectionName;
		SectionName = "Kick1";
		AnimInstance->Montage_JumpToSection(SectionName);
		StartAttack();
	}
}

void ACSPlayerCharacter::PlayHitReactMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName;
		SectionName = "HitReact1";
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ACSPlayerCharacter::StartAttack()
{
	if (CombatComponent)
	{
		if (HasAuthority())
		{
			CombatComponent->SetIsAttacking(true);
		}
		else
		{
			CombatComponent->ServerStartAttack();
		}
	}
}

void ACSPlayerCharacter::EndAttack()
{
	if (CombatComponent)
	{
		if (HasAuthority())
		{
			CombatComponent->SetIsAttacking(false);
		}
		else
		{
			CombatComponent->ServerEndAttack();
		}
	}
}