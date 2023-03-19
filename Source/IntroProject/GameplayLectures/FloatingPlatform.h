// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class INTROPROJECT_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FloatingPlatform")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, Category = "FloatingPlatform")
	FVector StartPoint;

	UPROPERTY(EditAnywhere, Category = "FloatingPlatform", meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, Category = "FloatingPlatform")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, Category = "FloatingPlatform")
	float InterpTime;

	FTimerHandle InterpTimer;

	UPROPERTY(EditAnywhere, Category = "FloatingPlatform")
	bool bInterping;

	float Distance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleInterping();

	void SwapVector(FVector& VecOne, FVector& VecTwo);
};
