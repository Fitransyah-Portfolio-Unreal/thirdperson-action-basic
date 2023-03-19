// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "IntroProject/CombatLectures/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "IntroProject/CombatLectures/Enemy.h"
#include "IntroProject/GameplayLectures/MainPlayerController.h"
#include "IntroProject/FinishingLectures/IntroSaveGame.h"
#include "IntroProject/FinishingLectures/ItemStorage.h"

// Sets default values
AMain::AMain()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create camera boom (pulls toward the player if there's a collisiong)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->TargetArmLength = 600.f;

    // Set size for collision capsule
    GetCapsuleComponent()->SetCapsuleSize(48.f, 105.f);

    // Create Follow Camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    // Attach camera to the end of the boom and let boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false;

    // Set our turn rate for input
    BaseTurnRate = 65.f;
    BaseLookupRate = 65.f;

    // Dont rotate when controller rotate
    // Let that just affect the camera
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // moves into direction of input
    GetCharacterMovement()->RotationRate = FRotator(0.f, 840.f, 0.f); // at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 650.f;
    GetCharacterMovement()->AirControl = 0.5f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1000.f;

    // Configure default player stats
    MaxHealth = 100.f;
    Health = 100.f;
    MaxStamina = 150.f;
    Stamina = 120.f;
    Coins = 0;

    // Configure default player sprint value
    RunningSpeed = 650.f;
    SprintingSpeed = 950.f;

    bShiftKeyDown = false;
    bLMBDown = false;

    // Inintialize Enums
    MovementStatus = EMovementStatus::EMS_Normal;
    StaminaStatus = EStaminaStatus::ESS_Normal;

    // Configure staminna status variable
    StaminaDrainRate = 25.f;
    MinsprintStamina = 50.f;

    InterpSpeed = 15.f;
    bInterpToEnemy = false;

    // Enemy HealthBar
    bHasCombatTarget = false;

    // Sprint Refining
    bMovingForward = false;
    bMovingRight = false;

    // Pause Menu
    bESCDown = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
    Super::BeginPlay();

    // Enemy HealthBar
    MainPlayerController = Cast<AMainPlayerController>(GetController());

    FString Map = GetWorld()->GetMapName();
    Map.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);

    if (Map != "SunTemple")
    {
        // Pause Menu
        LoadGameNoSwitch();

        if (MainPlayerController)
        {
            MainPlayerController->GameModeOnly();
        }
    }
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Death
    if (MovementStatus == EMovementStatus::EMS_Dead) return;

    float DeltaStamina = DeltaTime * StaminaDrainRate;
    
    switch (StaminaStatus)
    {
        // Normal Stamina State 
        case EStaminaStatus::ESS_Normal:
        if (bShiftKeyDown)
        {
            if (Stamina - DeltaStamina <= MinsprintStamina)
            {
                SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
                Stamina -= DeltaStamina;
            }
            else
            {
                Stamina -= DeltaStamina;
            }
            // Sprint Refining
            if (bMovingForward || bMovingRight)
            {
                SetMovementStatus(EMovementStatus::EMS_Sprinting);
            }
            else
            {
                SetMovementStatus(EMovementStatus::EMS_Normal);
            }
            //SetMovementStatus(EMovementStatus::EMS_Sprinting);
        }
        else // shift key up
        {
            if (Stamina + DeltaStamina >= MaxStamina)
            {
                Stamina = MaxStamina;
            }
            else
            {
                Stamina += DeltaStamina;
            }
            SetMovementStatus(EMovementStatus::EMS_Normal);
        }
        break;

        // Below Minimum State
        case EStaminaStatus::ESS_BelowMinimum:
        if (bShiftKeyDown)
        {
            if (Stamina - DeltaStamina <= 0.f)
            {
                SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
                Stamina = 0.f;
                SetMovementStatus(EMovementStatus::EMS_Normal);
            }
            else
            {
                Stamina -= DeltaStamina;
                // Sprint Refining
                if (bMovingForward || bMovingRight)
                {
                    SetMovementStatus(EMovementStatus::EMS_Sprinting);
                }
                else
                {
                    SetMovementStatus(EMovementStatus::EMS_Normal);
                }
                //SetMovementStatus(EMovementStatus::EMS_Sprinting);
            }
        }
        else // shift key up
        {
            if (Stamina + DeltaStamina >= MinsprintStamina)
            {
                SetStaminaStatus(EStaminaStatus::ESS_Normal);
                Stamina += DeltaStamina;
            }
            else
            {
                Stamina += DeltaStamina;
            }
            SetStaminaStatus(EStaminaStatus::ESS_Normal);
        }
        break;

        // Exhausted State
        case EStaminaStatus::ESS_Exhausted:
        if (bShiftKeyDown)
        {
            Stamina = 0.f;
        }
        else // shift key up
        {
            SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
            Stamina += DeltaStamina;
        }
        SetMovementStatus(EMovementStatus::EMS_Normal);
        break;

        // Exhausted Recovering state
        case EStaminaStatus::ESS_ExhaustedRecovering:
        if (Stamina + DeltaStamina >= MinsprintStamina)
        {
            SetStaminaStatus(EStaminaStatus::ESS_Normal);
            Stamina += DeltaStamina;
        }
        else
        {
            Stamina += DeltaStamina;
        }
        SetMovementStatus(EMovementStatus::EMS_Normal);
        break;

        default:
        ;
    }

    // Interpolation
    if (bInterpToEnemy && CombatTarget)
    {
        FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
        FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpSpeed);

        SetActorRotation(InterpRotation);
    }

    // Enemy HealthBar
    if (CombatTarget)
    {
        CombatTargetLocation = CombatTarget->GetActorLocation();

        if (MainPlayerController)
        {
            MainPlayerController->EnemyLocation = CombatTargetLocation;
        }
    }
}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    // Death
    //PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    //PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMain::StopJumping);

    PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);

    // Pause Menu
    //PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
    //PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
    PlayerInputComponent->BindAxis("Turn", this, &AMain::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AMain::LookUp);
    PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpRate);

    PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::ShiftKeyDown);
    PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::ShiftKeyUp);

    PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::LMBDown);
    PlayerInputComponent->BindAction("LMB", IE_Released, this, &AMain::LMBUp);

    PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &AMain::ESCDown);
    PlayerInputComponent->BindAction("ESC", IE_Released, this, &AMain::ESCUp);
}

void AMain::MoveForward(float Value)
{
    // Sprint Refining
    bMovingForward = false;
    
    // Death
    // Pause Menu
    // if ((Controller != nullptr) && (Value != 0.0f) && !bAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
    if (CanMove(Value))
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

        // Forwar direction base on camera looking (controller control spring arm + camera rotation)
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);

        // Sprint Refining
        bMovingForward = true;
    }
}

void AMain::MoveRight(float Value)
{
    // Sprint Refining
    bMovingRight = false;

    // Pause Menu
    // if ((Controller != nullptr) && (Value != 0.0f) && !bAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
    if (CanMove(Value))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

        // Right direction base on camera looking (controller control spring arm + camera rotation)
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);

        // Sprint Refining
        bMovingRight = true;
    }
}

void AMain::TurnAtRate(float Rate)
{
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpRate(float Rate)
{
    AddControllerPitchInput(Rate * BaseLookupRate * GetWorld()->GetDeltaSeconds());
}

// This will move into Take Damage function to make enemy stop attacking after player die
// By using damage causer
// not used anymore
void AMain::DecrementHealth(float Amount)
{
    
    if (Health - Amount <= 0.f)
    {
        Health -= Amount;
        Die();
    }
    else
    {
        Health -= Amount;
    }
}

void AMain::IncrementCoins(int32 Amount)
{
    Coins += Amount;
}

void AMain::Die()
{
    // Death
    if (MovementStatus == EMovementStatus::EMS_Dead) return;
    
    // Die montage play
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && CombatMontage)
    {
        AnimInstance->Montage_Play(CombatMontage, 1.f);
        AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
    }

    // Death
    SetMovementStatus(EMovementStatus::EMS_Dead);
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    //DecrementHealth(DamageAmount);

    if (Health - DamageAmount <= 0.f)
    {
        Health -= DamageAmount;
        Die();

        if (DamageCauser)
        {
            AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
            if (Enemy)
            {
                Enemy->bHasValidTarget = false;
            }
        }
    }
    else
    {
        Health -= DamageAmount;
    }

    return DamageAmount;
}

void AMain::SetMovementStatus(EMovementStatus Status)
{
    MovementStatus = Status;

    if (MovementStatus == EMovementStatus::EMS_Sprinting)
    {
        GetCharacterMovement()-> MaxWalkSpeed = SprintingSpeed;
    }
    else
    {
        GetCharacterMovement()-> MaxWalkSpeed = RunningSpeed;
    }
}

void AMain::ShiftKeyDown()
{
    bShiftKeyDown = true;
}


void AMain::ShiftKeyUp()
{
    bShiftKeyDown = false;
}

void AMain::ShowPickupLocations()
{
    // Debuug
    for (int32 i = 0; i < PickupLocations.Num(); i++)
    {
        UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
    }

  /*  for (FVector Location : PickupLocations)
    {
        UKismetSystemLibrary::DrawDebugSphere(this, PickupLocations[i], 25.f, 8, FLinearColor::Green, 10.f, 0.5f);
    }*/
}

void AMain::LMBDown()
{
    bLMBDown = true;

    // Pause Menu, if pause visible return
    if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;


    // Death
    // Its below the boolean to separate attack and equipping function only
    if (MovementStatus == EMovementStatus::EMS_Dead)
    {
        return;
    }

    if (ActiveOverlappingItem)
    {
        AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);

        if (Weapon)
        {
            Weapon->Equip(this);
        }
    }
    else if (EquippedWeapon)
    {
        Attack();
    }
}

void AMain::LMBUp()
{
    bLMBDown = false;
}

void AMain::SetEquippedWeapon(AWeapon* WeaponToSet)
{
    if(EquippedWeapon)
    {
        EquippedWeapon->Destroy();
    }

    EquippedWeapon = WeaponToSet;
}

void AMain::Attack()
{
    if (GetCharacterMovement()->IsFalling())
    {
        return;
    }
    
    // Death
    if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
    {
        bAttacking = true;
        SetInterpToEnemy(true);

        // Combat montage random play
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance && CombatMontage)
        {
            int32 Section = FMath::RandRange(0,1);
            switch (Section)
            {
                case 0:
                AnimInstance->Montage_Play(CombatMontage, 2.2f);
                AnimInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
                break;

                case 1:
                AnimInstance->Montage_Play(CombatMontage, 1.8f);
                AnimInstance->Montage_JumpToSection(FName("Attack_2"), CombatMontage);
                break;

                default:
                break;
                
            }
        }
    }
}

void AMain::AttackEnd()
{
    bAttacking = false;
    SetInterpToEnemy(false); // false

    if (bLMBDown)
    {
        Attack();
    }
}

void AMain::PlaySwingSound()
{
    // Play the swing sound here, please note that the property located in weapon class
    // This function is called by blueprint + notifier
    // better option is to put this playsound function in weapon class
    if (EquippedWeapon->SwingSound)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), EquippedWeapon->SwingSound);
    }
}

void AMain::SetInterpToEnemy(bool Interp)
{
    bInterpToEnemy = Interp;
}

FRotator AMain::GetLookAtRotationYaw(FVector Target)
{
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Target);
    FRotator LookAtRotationYaw(0.f, LookAtRotation.Yaw, 0.f);
    return LookAtRotationYaw;
}

void AMain::DeathEnd()
{
    GetMesh()->bPauseAnims = true;
    GetMesh()->bNoSkeletonUpdate = true;
}

void AMain::Jump()
{
    // Pause Menu, if pause visible return
    if (MainPlayerController) if (MainPlayerController->bPauseMenuVisible) return;
    
    if (bAttacking)
    {
        return;
    }

    if (MovementStatus != EMovementStatus::EMS_Dead)
    {
        Super::Jump();
    }
}

// Refining Gameplay (Combat) 
void AMain::UpdateCombatTarget()
{
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, EnemyFilter);

    if (OverlappingActors.Num() == 0) 
    {
        if (MainPlayerController)
        {
            MainPlayerController->RemoveEnemyHealthBar();
        }

        return;
    }

    AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

    if (ClosestEnemy)
    {
        FVector OurLocation = GetActorLocation();
        float MinDistance = (ClosestEnemy->GetActorLocation() - OurLocation).Size();

        for (auto Actor : OverlappingActors)
        {
            AEnemy* Enemy = Cast<AEnemy>(Actor);

            if (Enemy)
            {
                float DistanceToActor = (Enemy->GetActorLocation() - OurLocation).Size();

                if (DistanceToActor < MinDistance)
                {
                    MinDistance = DistanceToActor;
                    ClosestEnemy = Enemy;
                }
            } 
        }

        if (MainPlayerController)
        {
            MainPlayerController->DisplayEnemyHealthBar();
        }

        SetCombatTarget(ClosestEnemy);
        bHasCombatTarget = true;
    }
    
}

// Refining gameplay (Pickup)
void AMain::IncrementHealth(float Amount)
{
    if (Health + Amount >= MaxHealth)
    {
        Health = MaxHealth;
    }
    else
    {
        Health += Amount;
    }
}

// Level
void AMain::SwitchLevel(FName LevelName)
{
    UWorld* World = GetWorld();
    if (World)
    {
        FString CurrentLevel = World->GetMapName();

        // Dereferencing with literal string
        FName CurrentLevelName (*CurrentLevel);
        if (CurrentLevelName != LevelName)
        {
            UGameplayStatics::OpenLevel(World, LevelName);
        }
    }
}

void AMain::SaveGame()
{
    UIntroSaveGame* SaveGameInstance =  Cast<UIntroSaveGame>(UGameplayStatics::CreateSaveGameObject(UIntroSaveGame::StaticClass()));

    SaveGameInstance->CharacterStats.Health = Health;
    SaveGameInstance->CharacterStats.MaxHealth = MaxHealth;
    SaveGameInstance->CharacterStats.Stamina = Stamina;
    SaveGameInstance->CharacterStats.MaxStamina = MaxStamina;
    SaveGameInstance->CharacterStats.Coins = Coins;
    SaveGameInstance->CharacterStats.Location = GetActorLocation();
    SaveGameInstance->CharacterStats.Rotation = GetActorRotation();

    // save Level
    FString MapName = GetWorld()->GetMapName();
    MapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
    UE_LOG(LogTemp, Warning, TEXT("Mapname : %s"), *MapName);

    SaveGameInstance->CharacterStats.LevelName = MapName;

    // save weapon
    if (EquippedWeapon)
    {
        SaveGameInstance->CharacterStats.WeaponName = EquippedWeapon->Name;
    }

    UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerName, SaveGameInstance->UserIndex);

}

void AMain::LoadGame(bool SetPosition)
{
    UIntroSaveGame* LoadGameInstance = Cast<UIntroSaveGame>(UGameplayStatics::CreateSaveGameObject(UIntroSaveGame::StaticClass()));
    LoadGameInstance = Cast<UIntroSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));
    if (LoadGameInstance)
    {
        Health = LoadGameInstance->CharacterStats.Health;
        MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
        Stamina = LoadGameInstance->CharacterStats.Stamina;
        MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
        Coins = LoadGameInstance->CharacterStats.Coins;

    }

    // save weapon
    if (WeaponStorage)
    {
       AItemStorage* Weapons =  GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
       if (Weapons)
       {
           FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

           if (Weapons->WeaponMap.Contains(WeaponName))
           {
               AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
               WeaponToEquip->Equip(this);
           }

           // Alternative for prevent crash
           /*if (WeaponName != TEXT(""))
           {
               AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
               WeaponToEquip->Equip(this);
           }*/
          
       }
    }
    
    if (SetPosition)
    {
        SetActorLocation(LoadGameInstance->CharacterStats.Location);
        SetActorRotation(LoadGameInstance->CharacterStats.Rotation);
    }

    SetMovementStatus(EMovementStatus::EMS_Normal);
    GetMesh()->bPauseAnims = false;
    GetMesh()->bNoSkeletonUpdate = false;

    // save level
    if (LoadGameInstance->CharacterStats.LevelName != TEXT(""))
    {
        FName LevelName(*LoadGameInstance->CharacterStats.LevelName);

        SwitchLevel(LevelName);
    }
}


// Pause menu
void AMain::ESCDown()
{
    bESCDown = true;

    if (MainPlayerController)
    {
        MainPlayerController->TogglePauseMenu();
    }
}

void AMain::ESCUp()
{
    bESCDown = false;
}

bool AMain::CanMove(float Value)
{
    // can move if : Controller is not null, Value of axis not 0, not in attacking, not in dead status and pause meno notvisible
    if (MainPlayerController)
    {
        return (Controller != nullptr) &&
            (Value != 0.0f) && !bAttacking &&
            (MovementStatus != EMovementStatus::EMS_Dead) &&
            !MainPlayerController->bPauseMenuVisible;
    }
    return false;
}

void AMain::Turn(float Value)
{
    if (CanMove(Value))
    {
        AddControllerYawInput(Value);
    }
}

void AMain::LookUp(float Value)
{
    if (CanMove(Value))
    {
        AddControllerPitchInput(Value);
    }

}

void AMain::LoadGameNoSwitch()
{
    UIntroSaveGame* LoadGameInstance = Cast<UIntroSaveGame>(UGameplayStatics::CreateSaveGameObject(UIntroSaveGame::StaticClass()));
    LoadGameInstance = Cast<UIntroSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerName, LoadGameInstance->UserIndex));
    if (LoadGameInstance)
    {
        Health = LoadGameInstance->CharacterStats.Health;
        MaxHealth = LoadGameInstance->CharacterStats.MaxHealth;
        Stamina = LoadGameInstance->CharacterStats.Stamina;
        MaxStamina = LoadGameInstance->CharacterStats.MaxStamina;
        Coins = LoadGameInstance->CharacterStats.Coins;

    }
    else
    {
        SetMovementStatus(EMovementStatus::EMS_Normal);
        GetMesh()->bPauseAnims = false;
        GetMesh()->bNoSkeletonUpdate = false;
        return;
    }
    
    // save weapon
    if (WeaponStorage)
    {
        AItemStorage* Weapons = GetWorld()->SpawnActor<AItemStorage>(WeaponStorage);
        if (Weapons)
        {
            FString WeaponName = LoadGameInstance->CharacterStats.WeaponName;

            if (Weapons->WeaponMap.Contains(WeaponName))
            {
                AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapons->WeaponMap[WeaponName]);
                WeaponToEquip->Equip(this);
            }

            // Same here
        }
    }
    SetMovementStatus(EMovementStatus::EMS_Normal);
    GetMesh()->bPauseAnims = false;
    GetMesh()->bNoSkeletonUpdate = false;

   
}
