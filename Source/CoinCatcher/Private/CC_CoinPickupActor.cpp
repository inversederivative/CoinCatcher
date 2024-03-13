// Fill out your copyright notice in the Description page of Project Settings.


#include "CC_CoinPickupActor.h"

#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "NiagaraFunctionLibrary.h"
#include "CoinCatcher/CoinCatcherCharacter.h"

// Sets default values
ACC_CoinPickupActor::ACC_CoinPickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ColliderComponent = CreateDefaultSubobject<USphereComponent>("ColliderComponent");
	SetRootComponent(ColliderComponent);
	ColliderComponent->SetGenerateOverlapEvents(true);
	// Query is required for overlap (but not physics)
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ColliderComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// All Characters are Pawns
	ColliderComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ACC_CoinPickupActor::OnBeginOverlapComponentEvent);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	// Attach Mesh to the Collider Correctly
	MeshComponent->SetupAttachment(ColliderComponent);
	// No Collisions, and ignored on every channel
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetGenerateOverlapEvents(false);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovementComponent");
}

void ACC_CoinPickupActor::OnBeginOverlapComponentEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// If cast fails, will give nullptr
	if (!Cast<ACharacter>(OtherActor)) return;

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, PickupSound, GetActorLocation(), VolumeMultiplier);
	}

	if (OnPickupEffect)
	{
		const FVector Offset = GetActorUpVector() * PickupEffectSpawnOffset;
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, OnPickupEffect, OtherActor->GetActorLocation() + Offset);
	}

	ACoinCatcherCharacter* CoinCatcher = Cast<ACoinCatcherCharacter>(OtherActor);

	if (CoinCatcher)
	{
		// Call a function in your player character class to update the coin count.
		CoinCatcher->CollectCoin();
	}
		
	// If is valid character actor, let's destroy on Overlap!
	Destroy();

}


