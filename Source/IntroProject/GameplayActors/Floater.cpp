// Fill out your copyright notice in the Description page of Project Settings.


#include "Floater.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloater::AFloater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CustomStaticMesh"));

	InitialLocation = FVector (0.0f);
	PlacedLocation = FVector (0.0f);
	WorldOrigin = FVector(0.0f, 0.0f, 0.0f);
	InitialDirection = FVector(0.0f, 0.0f, 0.0f);
	Rotation = FRotator(0.0f);
	InitialForce = FVector(2000000.0f, 0.0f, 0.0f );
	InitialTorque = FVector(20000000.0f, 0.0f, 0.0f);

	bInitializeFloaterLocations = false;
	bShouldFloat = false;

	RunningTime = 0.f;

	A = 0.f;
	B = 0.f;
	C = 0.f;
	D = 0.f;
}

// Called when the game starts or when spawned
void AFloater::BeginPlay()
{
	Super::BeginPlay();
	
	PlacedLocation = GetActorLocation();

	float InitialX = FMath::FRandRange(-500, 500);
	float InitialY = FMath::FRandRange(-500, 500);
	float InitialZ = FMath::FRandRange(0, 500);

	InitialLocation.X = InitialX;
	InitialLocation.Y = InitialY;
	InitialLocation.Z = InitialZ;

	if (bInitializeFloaterLocations)
	{
		SetActorLocation(InitialLocation);
	}
	
	//StaticMesh->AddForce(InitialForce);
	//StaticMesh->AddTorqueInDegrees(InitialTorque);

	BaseZLocation = PlacedLocation.Z;
}

// Called every frame
void AFloater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bShouldFloat)
	{
		/*FHitResult HitResult;
		AddActorLocalOffset(InitialDirection, true, &HitResult);

		FVector HitLocation = HitResult.Location;*/
		FVector NewLocation = GetActorLocation();

		NewLocation.Z = BaseZLocation + A * FMath::Sin(B * RunningTime - C) + D; // Period = 2 * PI / ABS(B)

		SetActorLocation(NewLocation);
		RunningTime += DeltaTime;
	}

	//AddActorWorldOffset(InitialDirection);
	//AddActorLocalRotation(Rotation);
	//AddActorWorldRotation(Rotation);

}

