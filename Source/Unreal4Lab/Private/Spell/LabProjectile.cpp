// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabPawn.h"
#include "LabProjectile.h"


ALabProjectile::ALabProjectile(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
	, ConstantDamage(false)
{
	bReplicates = true;
	bReplicateInstigator = true;
	bReplicateMovement = true;

	bInitialized = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	PrimaryActorTick.bStartWithTickEnabled = false;

	CollisionComp = PCIP.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(1.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CollisionComp->bCanEverAffectNavigation = false;
	RootComponent = CollisionComp;

	MovementComp = PCIP.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->ProjectileGravityScale = 0.0f;

	DamageType = UDamageType::StaticClass();
}

void ALabProjectile::InitProjectile(const FVector& Direction, uint8 InTeamNum, int32 ImpactDamage, float InLifeSpan)
{
	MovementComp->OnProjectileStop.AddDynamic(this, &ALabProjectile::OnHit);
	MovementComp->Velocity = MovementComp->InitialSpeed * Direction;

	MyTeamNum = InTeamNum;
	RemainingDamage = ImpactDamage;
	SetLifeSpan(InLifeSpan);

	bInitialized = true;
}

void ALabProjectile::ReceiveActorBeginOverlap(class AActor* OtherActor)
{
	Super::ReceiveActorBeginOverlap(OtherActor);

	if (!bInitialized)
	{
		return;
	}

	if (HitActors.Find(OtherActor) != INDEX_NONE)
	{
		return;
	}
	HitActors.AddUnique(OtherActor);

	const ALabPawn* HitChar = Cast<ALabPawn>(OtherActor);
	if (HitChar && HitChar->GetTeamNum() > ELabTeam::Unknown && HitChar->GetTeamNum() != GetTeamNum())
	{
		FHitResult PawnHit;
		PawnHit.Actor = HitChar;
		PawnHit.Component = HitChar->CapsuleComponent.Get();
		PawnHit.bBlockingHit = true;
		PawnHit.Location = PawnHit.ImpactPoint = GetActorLocation();
		PawnHit.Normal = PawnHit.ImpactNormal = -MovementComp->Velocity.SafeNormal();

		OnHit(PawnHit);
	}
}

void ALabProjectile::OnHit(FHitResult const& HitResult)
{
	DealDamage(HitResult);
	OnProjectileHit(HitResult.Actor.Get(), HitResult.ImpactPoint, HitResult.ImpactNormal);

	if (RemainingDamage <= 0)
	{
		OnProjectileDestroyed();
		Destroy();
	}
}

void ALabProjectile::DealDamage(FHitResult const& HitResult)
{
	const ALabPawn* HitChar = Cast<ALabPawn>(HitResult.Actor.Get());
	const float PrevHealth = HitChar ? HitChar->GetHealth() : 0.0f;

	UGameplayStatics::ApplyPointDamage(HitResult.Actor.Get(), RemainingDamage, -HitResult.ImpactNormal, HitResult, NULL, this, UDamageType::StaticClass());

	if (HitChar && !ConstantDamage)
	{
		RemainingDamage -= FMath::TruncToInt(PrevHealth - HitChar->GetHealth());
	}
}

void ALabProjectile::LifeSpanExpired()
{
	OnProjectileDestroyed();

	Super::LifeSpanExpired();
}

uint8 ALabProjectile::GetTeamNum() const
{
	return MyTeamNum;
}
