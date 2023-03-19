// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "IntroProject/CharacterLectures/Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

APickup::APickup()
{

}

void APickup::OnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverLapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor)
	{
		AMain* Main = Cast<AMain>(OtherActor);
		if (Main)
		{
			// Refining gameplay (Pickup)
			OnPickupBP(Main);

			// Refining Item/Pickup/Explosive
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true, EPSCPoolMethod::None, true);
			}

			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}
			
			// Refining gameplay (Pickup)
			// Remove the increment  coin functions from here

			// Debug
			Main->PickupLocations.Add(GetActorLocation());

			Destroy();
		}
	}
	
}


void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}
