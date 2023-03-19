// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "IntroProject/CharacterLectures/Main.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "IntroProject/GameplayLectures/MainPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.f);
	
	// Extending Enemy
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(75.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetMesh(), FName("EnemySocket"));
	//CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));

	AIController = nullptr;

	bOverlappingCombatSphere = false;

	Health = 100.f;
	MaxHealth = 100.f;
	Damage = 10.f;

	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;
	DeathDelay = 3.f;

	bHasValidTarget = false;

}




// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverLapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);
	
	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverLapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverLapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	// Set no  collision at first because collision setup by notifier
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	// Refining Gameplay (Combat)
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Death
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->GetMovementComponent()->IsFalling())
			{
				return;
			}
			
			MoveToTarget(Main);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->GetMovementComponent()->IsFalling())
			{
				return;
			}
			// Death (Player)
			bHasValidTarget = false;


			// Interpolation 
			if (Main->CombatTarget == this)
			{
				Main->SetCombatTarget(nullptr);

				
			}
			
			// Enemy Health Bar
			Main->SetHasCombatTarget(false);

			// Refining Gameplay (Combat)
			// Enemy Health Bar
			//if (Main->MainPlayerController)
			//{
			//	Main->MainPlayerController->RemoveEnemyHealthBar();
			//}

			Main->UpdateCombatTarget();

			

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Death
	if (OtherActor && Alive())
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			// Death (Player)
			bHasValidTarget = true;

			if (Main->GetMovementComponent()->IsFalling())
			{
				return;
			}

			// Interpolation
			Main->SetCombatTarget(this);

			// Enemy HealthBar
			Main->SetHasCombatTarget(true);

			// Refining Gameplay (Combat)
			// This called in UpdateCombatTarget already
			//if (Main->MainPlayerController)
			//{
			//	Main->MainPlayerController->DisplayEnemyHealthBar();
			//}

			Main->UpdateCombatTarget();

			CombatTarget = Main;
			bOverlappingCombatSphere = true;

			// Delay Attack
			float  AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherComp)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			if (Main->GetMovementComponent()->IsFalling())
			{
				return;
			}

			//bOverlappingCombatSphere = false;
			//if (EnemyMovementStatus == EEnemyMovementStatus::EMS_Attacking)
			//{
			//	// If the enemy is not attacking we can continue to move to target
			//	// or we will call this on anim blueprint after attacking animation done
			//	MoveToTarget(Main);
			//	CombatTarget = nullptr;
			//}
			//else if (EnemyMovementStatus == EEnemyMovementStatus::EMS_MoveToTarget)
			//{
			//	MoveToTarget(Main);
			//	CombatTarget = nullptr;
			//}
			
			// Or just move to target in any movement status
			MoveToTarget(Main);
			CombatTarget = nullptr;

			// Refining Gameplay (Combat)
			if (Main->CombatTarget == this)
			{
				Main->SetCombatTarget(nullptr);
				Main->bHasCombatTarget = false;
				Main->UpdateCombatTarget();
			}

			if (Main->MainPlayerController)
			{
				USkeletalMeshComponent* MainMesh = Cast<USkeletalMeshComponent>(OtherComp);
				if (MainMesh)
				{
					Main->MainPlayerController->RemoveEnemyHealthBar();
				}
			}

			// Delay Attack
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::MoveToTarget(AMain* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	if (AIController && Alive())
	{

		// Creating struct for parameter #1
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		// The bigger the value the farther distance between each collision volume, eg : 25
		MoveRequest.SetAcceptanceRadius(30.f);

		// Creating typedef/smart pointer for parameter #2, this contain out information
		FNavPathSharedPtr NavPath;

		// MoveTo function for AI movement
		AIController->MoveTo(MoveRequest, &NavPath);



		// Debug 
		// path for AI to target

		//TArray<FNavPathPoint> PathPoints = NavPath->GetPathPoints();

		/*for (auto& Point : PathPoints)
		{
			FVector Location = Point.Location;

			UKismetSystemLibrary::DrawDebugSphere(this, Location, 75.f, 8, FLinearColor::Blue, 10.f, 2.f );
		}*/
	}
	
}


void AEnemy::CombatOnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Note here blood only happen when collide w/capsule component of Main
	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			// Spawn the blood particles here, please remember that variable property located in Main class
			if (Main->HitParticles)
			{
				const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName("TipSocket");
				if (TipSocket)
				{
					FVector SocketLocation = TipSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Main->HitParticles, SocketLocation, FRotator(0.f), false);
				}

			}

			if (Main->HitSound)
			{
				UGameplayStatics::PlaySound2D(GetWorld(), Main->HitSound);
			}

			// Damage
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(Main, Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	// Death
	if (Alive() && bHasValidTarget)
	{
		if (AIController)
		{
			AIController->StopMovement();

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}

		if (!bAttacking)
		{
			bAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.35f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;

	if (bOverlappingCombatSphere)
	{
		// Delay Attack
		float  AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
		//Attack();
	}
}

void AEnemy::PlaySwingSound()
{
	// This function is called by blueprint + notifier
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SwingSound);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health -= DamageAmount;
		Die(DamageCauser);
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.35f);
		AnimInstance->Montage_JumpToSection(FName("Death"),CombatMontage);
	}

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);
}

// Refining Gameplay (Combat)
void AEnemy::Die(AActor* Causer)
{
	Die();

	AMain* Main = Cast<AMain>(Causer);

	if (Main)
	{
		Main->UpdateCombatTarget();
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	// DeathEnd Issue :
	// 1. Enum not updating, need to create new VS files
	// 2. Put update enemy movement status after deactivate all collision
	// 3. Checking is Alive() in all colliding begin callback/function
	
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Dissappear, DeathDelay);
}

bool AEnemy::Alive()
{
	return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::Dissappear()
{
	Destroy();
}




