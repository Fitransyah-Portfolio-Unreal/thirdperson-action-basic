// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "IntroProject/CharacterLectures/Main.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "IntroProject/CombatLectures/Enemy.h"

AExplosive::AExplosive()
{
	Damage = 15.f;
}

void AExplosive::OnOverLapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverLapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex,bFromSweep, SweepResult);

	if (OtherActor)
	{
		// Check OtherActor if it is AMain or not, if yes will be stored in Main or return nullptr
		AMain* Main = Cast<AMain>(OtherActor);
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Main || Enemy)
		{
			// Refining Item/Pickup/Explosive
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.f), true, EPSCPoolMethod::None, true);
			}

			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			//Main->DecrementHealth(Damage);
			UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, DamageTypeClass);

			Destroy();
		}
	}
	
}


void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
