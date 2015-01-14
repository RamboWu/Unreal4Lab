// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Unreal4Lab.h"
#include "LabProjectile.h"
#include "LabBlueprintLibrary.h"


ULabBlueprintLibrary::ULabBlueprintLibrary(const FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
}

void ULabBlueprintLibrary::printDebugInfo(FString msg)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, msg);
}

ALabProjectile* ULabBlueprintLibrary::SpawnProjectileFromClass(UObject* WorldContextObject, TSubclassOf<ALabProjectile> ProjectileClass,
	const FVector& SpawnLocation, const FVector& ShootDirection, TEnumAsByte<ELabTeam::Type> OwnerTeam, int32 ImpactDamage, float LifeSpan, ALabPawn* InOwner)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObject(WorldContextObject);
	check(MyWorld);

	if (*ProjectileClass)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.bNoCollisionFail = true;
		ALabProjectile* const Proj = MyWorld->SpawnActor<ALabProjectile>(*ProjectileClass, SpawnLocation, ShootDirection.Rotation(), SpawnInfo);
		if (Proj)
		{
			Proj->OwnerPawn = InOwner;
			Proj->InitProjectile(ShootDirection, OwnerTeam, ImpactDamage, LifeSpan);
			return Proj;
		}
	}

	return NULL;
}
