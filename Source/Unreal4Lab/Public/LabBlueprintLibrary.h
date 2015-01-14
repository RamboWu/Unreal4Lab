// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "LabTypes.h"
#include "LabBlueprintLibrary.generated.h"

UCLASS()
class ULabBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	/**
	* Spawn a projectile.
	*
	* @param ProjectileBlueprint	The blueprint of the projectile to spawn.
	* @param SpawnLocation			Location to spawn projectile.
	* @param ShootDirection		Direction of travel for the projectile.
	* @param OwnerTeam				The team that owns the projectile.
	* @param ImpactDamage			The amount of damage the projectile will do.
	* @param LifeSpan				The lifespan of the projectile. (Defaults to 10).
	* @param InOwner				The Strategy building that 'owns' the projectile. (Defaults to NULL).
	*
	* @returns a pointer to the spawned projectile or NULL if the spawn was not successful.
	*/

	UFUNCTION(BlueprintCallable, Category = Game, meta = (WorldContext = "WorldContextObject"))
	static class ALabProjectile* SpawnProjectileFromClass(UObject* WorldContextObject, TSubclassOf<class ALabProjectile> ProjectileClass,
	const FVector& SpawnLocation, const FVector& ShootDirection, TEnumAsByte<ELabTeam::Type> OwnerTeam, int32 ImpactDamage, float LifeSpan = 10.0f, class ALabPawn* InOwner = NULL);

	static void printDebugInfo(FString msg);
};
