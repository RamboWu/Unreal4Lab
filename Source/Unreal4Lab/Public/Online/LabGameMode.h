// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "LabGameMode.generated.h"

UCLASS(minimalapi)
class ALabGameMode : public AGameMode
{
	GENERATED_UCLASS_BODY()


	/** Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string. */
	virtual void PreLogin(const FString& Options, const FString& Address, const TSharedPtr<class FUniqueNetId>& UniqueId, FString& ErrorMessage) override;

	/** starts match warmup */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** select best spawn point for player */
	virtual AActor* ChoosePlayerStart(AController* Player) override;

	/** always pick new random spawn */
	virtual bool ShouldSpawnAtStartSpot(AController* Player) override;

	/**
	* Helper function to test teams (null = not in friendly team).
	*
	*/
	static bool OnFriendlyTeam(const AActor* ActorA, const AActor* ActorB);

	/**
	* Helper function to test teams (null = not in friendly team).
	*/
	static bool OnEnemyTeam(const AActor* ActorA, const AActor* ActorB);

	/**
	* Modify the damage we want to apply to an actor. 怎么造成魔法伤害，怎么闪避，都是在GameMode里定义
	*
	* @param Damage			The damage
	* @param DamagedActor		The actor we wish to damage
	* @param DamageEvent		The event that caused the damage
	* @param EventInstigator
	* @param DamageCauser
	*
	* @returns The adjusted damage amount
	*/
	virtual float ModifyDamage(float Damage, AActor* DamagedActor, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const;

protected:
	/** check if player can use spawnpoint */
	virtual bool IsSpawnpointAllowed(APlayerStart* SpawnPoint, AController* Player) const;

	/** check if player should use spawnpoint */
	virtual bool IsSpawnpointPreferred(APlayerStart* SpawnPoint, AController* Player) const;
};



