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

}

void ACSPlayerCharacter::ComboCheck()
{
	CombatComponent->CanComboChange(true);
	DuringAttack();
}

void ACSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogTemp, Verbose, TEXT("ACSPlayerCharacter::Tick - MaxWalkSpeed: %f"), GetCharacterMovement()->MaxWalkSpeed);

}

void ACSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (MappingContext)
				{
					Subsystem->ClearAllMappings();
					Subsystem->AddMappingContext(MappingContext, 0);
				}
			}
		}
	}

	// --- 함수 호출 로그 추가 ---
	UE_LOG(LogTemp, Warning, TEXT("ACSPlayerCharacter::SetupPlayerInputComponent --- Called for %s"), *GetName());
	// ------------------------

	// CastChecked 대신 Cast 를 사용하여 실패 시 로그 남기기
	// if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// --- 캐스팅 성공 로그 ---
		UE_LOG(LogTemp, Log, TEXT("SetupPlayerInputComponent: Successfully cast PlayerInputComponent to UEnhancedInputComponent. Binding actions..."));
		// --------------------

		// --- 각 액션 바인딩 시도 및 로그 추가 ---
		if (MovementAction) {
			EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ACSPlayerCharacter::Move);
			UE_LOG(LogTemp, Log, TEXT("SetupPlayerInputComponent: Bound MovementAction."));
		}
		else { UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: MovementAction is NULL!")); }

		if (JumpAction) {
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACSPlayerCharacter::Jump);
			UE_LOG(LogTemp, Log, TEXT("SetupPlayerInputComponent: Bound JumpAction."));
		}
		else { UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: JumpAction is NULL!")); }

		if (CrouchAction) {
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ACSPlayerCharacter::CrouchStart);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ACSPlayerCharacter::CrouchEnd);
			UE_LOG(LogTemp, Log, TEXT("SetupPlayerInputComponent: Bound CrouchAction (Started/Completed)."));
		}
		else { UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: CrouchAction is NULL!")); }

		if (GuardAction) {
			EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Triggered, this, &ACSPlayerCharacter::GuardStart);
			EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, this, &ACSPlayerCharacter::GuardEnd);
			UE_LOG(LogTemp, Log, TEXT("SetupPlayerInputComponent: Bound GuardAction (Triggered/Completed)."));
		}
		else { UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: GuardAction is NULL!")); }

		if (DodgeAction) {
			EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ACSPlayerCharacter::DodgeStart);
			UE_LOG(LogTemp, Log, TEXT("SetupPlayerInputComponent: Bound DodgeAction."));
		}
		else { UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: DodgeAction is NULL!")); }
		// -----------------------------------
	}
	else
	{
		// --- 캐스팅 실패 로그 ---
		UE_LOG(LogTemp, Error, TEXT("SetupPlayerInputComponent: Failed to cast PlayerInputComponent to UEnhancedInputComponent! Input bindings will fail."));
		// --------------------
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
	UE_LOG(LogTemp, Warning, TEXT("ACSPlayerCharacter::Move - Current ActionState: %d"), (int32)ActionState); // 로그 추가

	if (ActionState == ECharacterTypes::ECT_Launch && !GetCharacterMovement()->IsFalling())
	{
		CombatComponent->CanComboChange(false);
		ServerSetStandUpState(EStandUpType::EST_StandUp);
		return;
	}

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
		StartAttack(PlayMontage, Section);
		CombatComponent->CanComboChange(false);
		AnimInstance->Montage_Play(PlayMontage);
		FName SectionName;
		SectionName = Section;
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ACSPlayerCharacter::GuardStart()
{
	if (ActionState == ECharacterTypes::ECT_Launch) return;

	if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		ServerSetActionState(ECharacterTypes::ECT_Defending);
	}
}

void ACSPlayerCharacter::GuardEnd()
{
	if (ActionState == ECharacterTypes::ECT_Launch) return;

	ServerSetActionState(ECharacterTypes::ECT_Unoccupied);
}

void ACSPlayerCharacter::DodgeStart()
{
	if (ActionState == ECharacterTypes::ECT_Unoccupied)
	{
		Server_PerformDodge();
	}
}

void ACSPlayerCharacter::DodgeEnd()
{
	if (ActionState == ECharacterTypes::ECT_Dodge)
	{
		Server_FinishDodge();
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

void ACSPlayerCharacter::Server_PerformDodge_Implementation()
{
	if (ActionState == ECharacterTypes::ECT_Unoccupied)
	{
		ActionState = ECharacterTypes::ECT_Dodge;
		OnRep_ActionState();

		Multicast_PlayDodgeMontage();
	}
}

void ACSPlayerCharacter::Multicast_PlayDodgeMontage_Implementation()
{
	if (DodgeMontage)
	{
		UAnimInstance* AnimInstance = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
		if (AnimInstance)
		{
			AnimInstance->Montage_Play(DodgeMontage, 1.f);
		}
	}
}

void ACSPlayerCharacter::Server_FinishDodge_Implementation()
{
	if (ActionState == ECharacterTypes::ECT_Dodge)
	{
		ActionState = ECharacterTypes::ECT_Unoccupied;
		OnRep_ActionState();
	}
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
	if (!AttributeComponent->IsAlive()) return;

	if (CombatComponent)
	{
		CombatComponent->ResetComboData();
		if (HasAuthority())
		{
			CombatComponent->SetIsAttacking(false);
			ActionState = ECharacterTypes::ECT_Unoccupied;
		}
	}
	ActionState = ECharacterTypes::ECT_Unoccupied;
	ServerSetStandUpState(EStandUpType::EST_Nomal);
	OnRep_ActionState();
}

void ACSPlayerCharacter::ComboStateChange()
{
	CombatComponent->CanComboChange(false);
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
	if (ActionState == ECharacterTypes::ECT_Unoccupied)
	{
		float TargetYaw = (FacingDirection == EFacingDirection::EFD_FacingRight) ? -90.f : 90.f;
		FRotator NewRotation = FRotator(0.f, TargetYaw, 0.f);
		SetActorRotation(NewRotation);
	}
}

void ACSPlayerCharacter::OnRep_FacingDirection()
{
	UpdateRotation();
}

void ACSPlayerCharacter::ActivateSuddenDeathMode()
{
	if (HasAuthority())
	{
		if (CombatComponent && !CombatComponent->IsSuddenDeathActive())
		{
			CombatComponent->SetSuddenDeathActive(true);
		}
	}
}
