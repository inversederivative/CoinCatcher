// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CC_CoinPickupActor.generated.h"

class UStaticMeshComponent;
class URotatingMovementComponent;
class USphereComponent;
class UNiagaraSystem;
class USoundBase;


UCLASS()
class COINCATCHER_API ACC_CoinPickupActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ACC_CoinPickupActor();

	// Custom Overlap Function
	UFUNCTION()
	void OnBeginOverlapComponentEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
protected:

	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	TObjectPtr<UStaticMeshComponent> MeshComponent; 

	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	TObjectPtr<URotatingMovementComponent> RotatingMovementComponent; 

	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	TObjectPtr<USphereComponent> ColliderComponent; 

	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	TObjectPtr<UNiagaraSystem> OnPickupEffect; 
	
	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	TObjectPtr<USoundBase> PickupSound;

	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	float VolumeMultiplier {0.5f};

	UPROPERTY(EditDefaultsOnly, Category="Coin Collector")
	float PickupEffectSpawnOffset {90.0f};
	
};

