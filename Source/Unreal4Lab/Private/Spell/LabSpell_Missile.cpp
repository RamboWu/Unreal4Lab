// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPawn.h"
#include "LabBlueprintLibrary.h"
#include "LabSpell_Missile.h"


ALabSpell_Missile::ALabSpell_Missile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	HasActive = true;
	bInLaunchMissile = false;
	static ConstructorHelpers::FObjectFinder<UClass> ProjectileBPClass(TEXT("Class'/Game/Projectiles/Projectile_arbalest.Projectile_arbalest_C'"));

	if (ProjectileBPClass.Object != NULL)
	{
		MissileProjectileClass = ProjectileBPClass.Object;
	}
}

void ALabSpell_Missile::PerformCast()
{
	// Only launch the missiles on the server
	if (HasAuthority() && !bInLaunchMissile)
	{
		MissilesLaunched = 0;
		MissionTargetLocation = TargetLocation;
		bInLaunchMissile = true;
		GetWorldTimerManager().SetTimer(this, &ALabSpell_Missile::LaunchMissileTimer, 0.05f, false);
		//SetTimer(0.05f, true, NameOf(LaunchMissileTimer));
	}
}

int32 ALabSpell_Missile::GetMissileCount()
{
	if (MissileAmount.Num() == 0)
	{
		return 1;
	}

	if (Level < 0)
	{
		return 0;
	}

	if (MissileAmount.Num() <= Level)
	{
		// undefined - return best estimate
		return MissileAmount[MissileAmount.Num() - 1];
	}

	return MissileAmount[Level];
}

void ALabSpell_Missile::LaunchMissileTimer()
{
	ALabPawn* pawn = ::Cast<ALabPawn>(PawnOwner);
	if (!pawn)
	{
		ULabBlueprintLibrary::printDebugInfo("ALabSpell_Missile::LaunchMissileTimer No Pawn Owner");
	}
	
	//FHitResult Hit;
	//GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	//FVector Direction = (Hit.ImpactPoint - pawn->GetActorLocation());

	FRotator face_rotation;
	face_rotation.Yaw = pawn->GetActorRotation().Yaw;


	ULabBlueprintLibrary::SpawnProjectileFromClass(
		GetWorld(), MissileProjectileClass, PawnOwner->GetActorLocation(), 
		pawn->GetActorRotation().Vector(), (TEnumAsByte<ELabTeam::Type>)pawn->GetTeamNum(), pawn->PawnReplicationInfo.Damage, 10.f, pawn);

	// Count how many missiles have been launched
	MissilesLaunched++;
	// Clear the timer if the amount of missiles have been launched
	if (MissilesLaunched >= GetMissileCount())
	{
		bInLaunchMissile = false;
	}
	else
	{
		GetWorldTimerManager().SetTimer(this, &ALabSpell_Missile::LaunchMissileTimer, 0.05f, false);
	}
}