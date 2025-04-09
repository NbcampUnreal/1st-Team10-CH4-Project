#include "Components/CSCombatComponent.h"

#include "AI/Character/AIBaseCharacter.h"
#include "AI/Controller/AIBaseController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/CSPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CSAttributeComponent.h"

UCSCombatComponent::UCSCombatComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicated(true);

    // Reset Combo Data
    iCombo_1_Cnt = 0;
    iCombo_2_Cnt = 0;
    bCanCombo = true;
}

void UCSCombatComponent::Server_PerformHitCheck_Implementation(FName TraceStartName, FName TraceEndName)
{
    ACharacter* Owner = Cast<ACharacter>(GetOwner());
    if (!Owner) return;

    const FName FirstSocketName = TraceStartName;
    const FVector SocketStart = Owner->GetMesh()->GetSocketLocation(FirstSocketName);
	const FName SecondSocketName = TraceEndName;
    const FVector TraceStart = SocketStart;
    const FVector TraceEnd = Owner->GetMesh()->GetSocketLocation(SecondSocketName);
    const float TraceRadius = 30.0f;

    // Sphere Trace for Objects
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(Owner);

    FHitResult HitResult;

    // Debug Lines
    const bool bDrawDebug = true;
    const FColor TraceColor = FColor::Red;
    const FColor HitColor = FColor::Green;
    const float DebugDrawDuration = 2.0f;

    bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
        GetWorld(),
        TraceStart,
        TraceEnd,
        TraceRadius,
        ObjectTypes,
        false,
        ActorsToIgnore,
        bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
        HitResult,
        true,
        TraceColor,
        HitColor,
        DebugDrawDuration
    );

    if (bHit && HitResult.GetActor())
    {
        AActor* HitActor = HitResult.GetActor();
        bool bAlreadyHit = HitActorsThisAttack.ContainsByPredicate([&](const TWeakObjectPtr<AActor>& WeakPtr)
        {
            return WeakPtr.Get() == HitActor;
        });

        if (!bAlreadyHit)
        {
            HitActorsThisAttack.AddUnique(HitActor);

            ACSBaseCharacter* VictimCharacter = Cast<ACSBaseCharacter>(HitActor);
            if (VictimCharacter && VictimCharacter->IsBlocking())
            {
                return;
            }
    
            float DamageToApply = 10.f;

            AController* InstigatorController = Owner->GetController();
            AActor* DamageCauser = Owner;

            UCSAttributeComponent* VictimAttributes = HitActor->FindComponentByClass<UCSAttributeComponent>();
            if (VictimAttributes)
            {
                VictimAttributes->ReceiveDamage(DamageToApply, InstigatorController, DamageCauser);
            }

            if (VictimCharacter)
            {
                AAIBaseController* AIController = Cast<AAIBaseController>(VictimCharacter->GetController());
                if (AIController)
                {
                    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
                    if (BB)
                    {
                        float RandomValue = FMath::FRand(); // 0.0 ~ 1.0
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

                VictimCharacter->PlayHitReactMontage();
            }
        }
    }
}


void UCSCombatComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UCSCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UCSCombatComponent, bIsAttacking);
}

void UCSCombatComponent::Combo1CntIncrease()
{
    iCombo_1_Cnt++;
}

int32 UCSCombatComponent::GetCombo1Cnt()
{
    return iCombo_1_Cnt;
}

void UCSCombatComponent::Combo2CntIncrease()
{
    iCombo_2_Cnt++;
}

int32 UCSCombatComponent::GetCombo2Cnt()
{
    return iCombo_2_Cnt;
}

void UCSCombatComponent::CanComboChange(bool Check)
{
    bCanCombo = Check;
}

bool UCSCombatComponent::GetCanCombo()
{
    return bCanCombo;
}

void UCSCombatComponent::ResetComboData()
{
    iCombo_1_Cnt = 0;
    iCombo_2_Cnt = 0;
    bCanCombo = true;
}

void UCSCombatComponent::ClearHitActors()
{
    UE_LOG(LogTemp, Error, TEXT("!!!!!!!!!! ClearHitActors CALLED !!!!!!!!!!"));
	HitActorsThisAttack.Empty();
}

void UCSCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCSCombatComponent::SetIsAttacking(bool bAttacking)
{
    if (GetOwnerRole() == ROLE_Authority)
    {
        bool bOldValue = bIsAttacking;
        bIsAttacking = bAttacking;

        if (bOldValue != bIsAttacking)
        {
            OnRep_IsAttacking();
            
        }
    }
    else
    {
        // 클라일 경우 서버에 알림
        ServerSetIsAttacking(bAttacking);
    }
}

void UCSCombatComponent::MultiSetMontageData_Implementation(UAnimMontage* PlayMontage, FName Section)
{
    ServerPlayMontage = PlayMontage;
    ServerSection = Section;

    ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(GetOwner());
    if (!Character) return;

    if (!Character->HasAuthority() && !Character->IsLocallyControlled())
    {
        UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
        if (AnimInstance && ServerPlayMontage)
        {
            AnimInstance->Montage_Play(ServerPlayMontage);
            AnimInstance->Montage_JumpToSection(ServerSection);

            Character->StopMovement();
        }
    }
}

void UCSCombatComponent::ServerSetMontageData_Implementation(UAnimMontage* PlayMontage, FName Section)
{
    MultiSetMontageData(PlayMontage, Section);
    ServerStartAttack();
}

void UCSCombatComponent::OnRep_IsAttacking()
{
    ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(GetOwner());
    if (Character)
    {
        if (bIsAttacking)
        {
            Character->StopMovement();
        }
    }
}

void UCSCombatComponent::ServerStartAttack_Implementation()
{
    SetIsAttacking(true);
    ClearHitActors();

    ACSBaseCharacter* Character = Cast<ACSBaseCharacter>(GetOwner());

    if (Character && Character->HasAuthority() && ServerPlayMontage)
    {
        UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();

        if (AnimInstance)
        {
            AnimInstance->Montage_Play(ServerPlayMontage);
            AnimInstance->Montage_JumpToSection(ServerSection);

            Character->StopMovement();
        }
    }
}

void UCSCombatComponent::ServerEndAttack_Implementation()
{
    SetIsAttacking(false);
    ClearHitActors();
}

void UCSCombatComponent::PerformAttack(UAnimMontage* Montage, FName SectionName)
{
    if (!Montage) return;
    
    ServerSetMontageData(Montage, SectionName);
}

void UCSCombatComponent::ServerSetIsAttacking_Implementation(bool bAttacking)
{
    SetIsAttacking(bAttacking);
}

bool UCSCombatComponent::ServerSetIsAttacking_Validate(bool bAttacking)
{
    return true;
}