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

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SceneComp->SetupAttachment(RootComponent);

	AttributeComponent = CreateDefaultSubobject<UCSAttributeComponent>(TEXT("AttributeComponent"));
	CombatComponent = CreateDefaultSubobject<UCSCombatComponent>(TEXT("CombatComponent"));

	GetCharacterMovement()->MaxWalkSpeed = 600.f;
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

void ACSPlayerCharacter::ComboCheck()
{
	CombatComponent->CanComboChange(true);
	DuringAttack();
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
		//EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ACSPlayerCharacter::PlayAttackMontage);
	}
}

void ACSPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACSPlayerCharacter, FacingDirection);
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
	if (ActionState == ECharacterTypes::ECT_Dead || ActionState != ECharacterTypes::ECT_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FRotator Rotation = Controller->GetControlRotation();
	const FVector ForwardDirection = FVector(0.f, -1.f, 0.f); // Y축 방향
	AddMovementInput(ForwardDirection, MovementVector.X);

	UpdateFacingDirection(MovementVector.X);
}

void ACSPlayerCharacter::PlayPlayerMontage(UAnimMontage* PlayMontage, FName Section)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && PlayMontage)
	{
		CombatComponent->CanComboChange(false);
		AnimInstance->Montage_Play(PlayMontage);
		FName SectionName;
		SectionName = Section;
		AnimInstance->Montage_JumpToSection(SectionName);
		StartAttack(PlayMontage, Section);
	}
}

void ACSPlayerCharacter::StopMovement_Implementation()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void ACSPlayerCharacter::ServerSpawnProjectile_Implementation(ACSPlayerCharacter* SpawnPlayer)
{
	MultiSpawnProjectile(SpawnPlayer);
}

void ACSPlayerCharacter::MultiSpawnProjectile_Implementation(ACSPlayerCharacter* SpawnPlayer)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = SpawnPlayer;
	SpawnParams.Instigator = SpawnPlayer;

	GetWorld()->SpawnActor<AActor>(
		CastProjectile,
		SceneComp->GetComponentLocation(),
		SceneComp->GetComponentRotation(),
		SpawnParams
	);
}

void ACSPlayerCharacter::StartAttack(UAnimMontage* PlayMontage, FName Section)
{
	if (CombatComponent)
	{
		if (HasAuthority())
		{
			CombatComponent->ClearHitActors();

			ActionState = ECharacterTypes::ECT_Attacking;
			OnRep_ActionState();
			StopMovement();
			CombatComponent->MultiSetMontageData(PlayMontage, Section);
			CombatComponent->SetIsAttacking(true);
		}

		else
		{
			CombatComponent->ServerSetMontageData(PlayMontage, Section);
		}
	}
}

void ACSPlayerCharacter::DuringAttack()
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

void ACSPlayerCharacter::EndAttack()
{
	//ComboReset();

	if (CombatComponent)
	{
		CombatComponent->ResetComboData();

		if (HasAuthority())
		{
			CombatComponent->SetIsAttacking(false);
		}
	}
	ActionState = ECharacterTypes::ECT_Unoccupied;
	OnRep_ActionState();
}

void ACSPlayerCharacter::UpdateFacingDirection(float XInput)
{
	EFacingDirection CurrentDirection = FacingDirection;

	if (XInput > 0.f)
	{
		CurrentDirection = EFacingDirection::EFD_FacingRight;
	}
	else if (XInput < 0.f)
	{
		CurrentDirection = EFacingDirection::EFD_FacingLeft;
	}

	if (CurrentDirection != FacingDirection && IsLocallyControlled())
	{
		ServerSetFacingDirection(CurrentDirection);
	}
}

void ACSPlayerCharacter::ServerSetFacingDirection_Implementation(EFacingDirection NewDirection)
{
	if (HasAuthority())
	{
		FacingDirection = NewDirection;
		OnRep_FacingDirection();
	}
}

void ACSPlayerCharacter::UpdateRotation()
{
	float TargetYaw = (FacingDirection == EFacingDirection::EFD_FacingRight) ? -90.f : 90.f;
	FRotator NewRotation = FRotator(0.f, TargetYaw, 0.f);
	SetActorRotation(NewRotation);
}

void ACSPlayerCharacter::OnRep_FacingDirection()
{
	UpdateRotation();
}
