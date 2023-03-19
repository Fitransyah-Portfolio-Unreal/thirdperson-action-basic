// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA (DisplayName = "Sprinting"),
	EMS_Dead UMETA (DisplayName = "Dead"),

	EMS_MAX UMETA (DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class INTROPROJECT_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();

	/**
	*
	* ----------------------Player Camera Component
	*
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookupRate;

	// Called for forward/backwards input
	void MoveForward(float Value);

	// Called for side to side input
	void MoveRight(float Value);

	/** Called via input to turn at a given rate
	* @param Rate this is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void TurnAtRate(float Rate);

	/** Called via input to look up/down at a given rate
	* @param Rate this is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	*/
	void LookUpRate(float Rate);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/**
	* 
	* ------------------------Player Stats
	* 
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "PlayerStats")
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerStats")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "PlayerStats")
	float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerStats")
	int32 Coins;

	void DecrementHealth(float Amount);

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 Amount);

	void Die();

	// Damage
	virtual float TakeDamage(float DamageAmount,struct FDamageEvent const& DamageEvent,class AController* EventInstigator,AActor* DamageCauser) override;


	/**
	*
	* ------------------------Player Sprint Mechanic
	*
	*/
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	// Set the movememnt status and running speed
	void SetMovementStatus(EMovementStatus Status);

	// SetStamina Status for Stamina Bar
	FORCEINLINE void SetStaminaStatus(EStaminaStatus Status) { StaminaStatus = Status; }

	// variable for running/sprinting
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float RunningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float SprintingSpeed;

	bool bShiftKeyDown;

	// variable for stamina status

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinsprintStamina;


	// pressed down to enable sprinting
	void ShiftKeyDown();

	// released to disable sprinting
	void ShiftKeyUp();

	/**
	*
	* ------------------------Debugging
	*
	*/

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocations();

	/**
	*
	* ------------------------Weapon setting
	*
	*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	// Setter to weapon
	void SetEquippedWeapon(AWeapon* WeaponToSet);

	// Setter for overlapping item
	FORCEINLINE void SetActiveOverlappingItem(AItem* ItemToSet) { ActiveOverlappingItem = ItemToSet; }

	// Getter to weapon
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	// Getter for overlapping item
	FORCEINLINE AItem* GetActiveOverlappingItem() { return ActiveOverlappingItem; }

	bool bLMBDown;

	void LMBDown();
	void LMBUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	*
	* ----------------------Combat
	*
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly , Category = "Combat")
	bool bAttacking;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UFUNCTION(BlueprintCallable)
	void PlaySwingSound();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* HitSound;

	// Interpolation
	float InterpSpeed;
	bool bInterpToEnemy;
	void SetInterpToEnemy(bool Interp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* Target) {CombatTarget = Target;}

	FRotator GetLookAtRotationYaw(FVector Target);

	// Enemy HealthBar
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasCombatTarget;

	FORCEINLINE void SetHasCombatTarget(bool HasTarget) { bHasCombatTarget = HasTarget;}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Controller")
	class AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	// Death
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	virtual void Jump() override;

	// Sprint Refining
	bool bMovingForward;
	bool bMovingRight;

	// Refining Gameplay (Combat)
	void UpdateCombatTarget();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	// Refining gameplay (Pickup)
	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float Amount);

	// Level
	void SwitchLevel(FName LevelName);

	// save game
	UFUNCTION(BlueprintCallable)
	void SaveGame();
	
	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	// save weapon
	UPROPERTY(EditDefaultsOnly, Category = "SaveGameData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	// Pause Menu
	bool bESCDown;
	void ESCDown();
	void ESCUp();

	bool CanMove(float Value);

	// Call for yaw rotation
	void Turn(float Value);

	// Call for pitch rotation
	void LookUp(float Value);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();
};
