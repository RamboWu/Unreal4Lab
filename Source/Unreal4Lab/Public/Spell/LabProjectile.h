// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LabTeamInterface.h"
#include "LabProjectile.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UNREAL4LAB_API ALabProjectile : public AActor, public ILabTeamInterface
{
	GENERATED_UCLASS_BODY()

	/** movement component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	TSubobjectPtr<UProjectileMovementComponent> MovementComp;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	TSubobjectPtr<USphereComponent> CollisionComp;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile)
	class ALabPawn* OwnerPawn;

	/** if set, damage will be constant through lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Damage)
	bool ConstantDamage;

	/** blueprint event: projectile hit something */
	UFUNCTION(BlueprintImplementableEvent, Category = Projectile)
	virtual void OnProjectileHit(AActor* HitActor, const FVector& HitLocation, const FVector& HitNormal);

	/** blueprint event: projectile hit something */
	UFUNCTION(BlueprintImplementableEvent, Category = Projectile)
	virtual void OnProjectileDestroyed();

	/** initial setup */
	void InitProjectile(const FVector& ShootDirection, uint8 InTeamNum, int32 ImpactDamage, float InLifeSpan);

	/** handle hit */
	UFUNCTION()
	void OnHit(const FHitResult& HitResult);

	// Begin Actor interface

	virtual void LifeSpanExpired() override;

	/** handle touch to detect enemy pawns */
	virtual void ReceiveActorBeginOverlap(class AActor* OtherActor) override;

	// End Actor interface


	// Begin StrategyTeamInterface interface

	/** [IStrategyTeamInterface] get team number */
	virtual uint8 GetTeamNum() const override;

	// End StrategyTeamInterface interface

protected:
	/** deal damage */
	void DealDamage(FHitResult const& HitResult);

	/** current team number */
	uint8 MyTeamNum;

	/** remaining damage value */
	int32 RemainingDamage;

	/** list of just hit actors */
	TArray<class AActor*> HitActors;

	/** true, if projectile was initialized */
	bool bInitialized;
};
