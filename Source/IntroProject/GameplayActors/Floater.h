// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floater.generated.h"

UCLASS(meta = (ShortTooltip = "Class that created for learning purposes"))
class INTROPROJECT_API AFloater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloater();


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ActorMeshComponent")
	UStaticMeshComponent* StaticMesh;

	// Location used by SetActorLocation() when BeginPlpay() is callled
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialLocation;

	// Location of the Actor when dragged in from editor
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector PlacedLocation;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "FloaterVariables")
	FVector WorldOrigin;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloaterVariables")
	FVector InitialDirection;

	UPROPERTY(EditInstanceOnly,  BlueprintReadOnly, Category = "FloaterVariables")
	FRotator Rotation;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialForce;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FloaterVariables")
	FVector InitialTorque;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloaterVariables")
	bool bShouldFloat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FloaterVariables")
	bool bInitializeFloaterLocations;

private: 

    float RunningTime;

	float BaseZLocation;

public :
	// Amplitude - how much we oscillate up and down
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloaterVariables")
	float A;

	// Period is 2 * PI / (absolute value of B), make oscillation faster or slower
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloaterVariables")
	float B;

	// Phase Shift is (C / B) get the other sin starting point
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloaterVariables")
	float C;

	// Vertical Shift is D can move the oscillation starting point plus or minus
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FloaterVariables")
	float D;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
