// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Spell/LabSpell.h"
#include "LabSpell_Missile.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabSpell_Missile : public ALabSpell
{
	GENERATED_UCLASS_BODY()




	UPROPERTY()
	TSubclassOf<class ALabProjectile> MissileProjectileClass;

	UPROPERTY()
	TArray<int32> MissileAmount;

protected:
	// Counter of how many missiles have been launched
	int32 MissilesLaunched;
	// Current target location for the missiles
	FVector MissionTargetLocation;

	//Only Server can spawn missile
	virtual void PerformCast() override;

	//return current amount
	int32 GetMissileCount();
	//Only Server can do this function
	void LaunchMissileTimer();

	bool bInLaunchMissile;
};
