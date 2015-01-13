// Fill out your copyright notice in the Description page of Project Settings.

#include "Unreal4Lab.h"
#include "LabGameMode.h"
#include "LabBlueprintLibrary.h"
#include "LabPawnReplicationInfo.h"
#include "LabStatsModifier.h"
#include "LabPawn.h"


ALabPawn::ALabPawn(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP), 
	BaseHealth(100),
	BaseMana(100),
	Health(0),
	Mana(0),
	BaseAttackDistance(100),
	BaseSightDistance(300),
	BaseDamage(30),
	StatsModifier(NULL)
{
	bReplicates = true;
	JustSpawned = true;
	bIsDying = false;
}

void ALabPawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALabPawn, m_team_num);
	DOREPLIFETIME(ALabPawn, Health);
	DOREPLIFETIME(ALabPawn, Mana);
	DOREPLIFETIME(ALabPawn, PawnReplicationInfo);
	DOREPLIFETIME(ALabPawn, LastTakeHitInfo);
}

uint8 ALabPawn::GetTeamNum() const
{
	return m_team_num;
}

void ALabPawn::SetTeamNum(uint8 NewTeamNum)
{
	m_team_num = (TEnumAsByte<ELabTeam::Type>)NewTeamNum;
}

int32 ALabPawn::GetHealth() const
{
	return Health;
}

float ALabPawn::GetHealthPercentage() const
{
	return ((float)Health / GetMaxHealth());
}

int32 ALabPawn::GetMaxHealth() const
{
	return BaseHealth;
}

int32 ALabPawn::GetAttackRange() const
{
	return BaseAttackDistance;
}

int32 ALabPawn::GetSightDistance() const
{
	return BaseSightDistance;
}


void ALabPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		// Recalculates the pawn stats
		RecalculateStats();

		// Update the health and mana
// 		if (PawnReplicationInfo != NULL)
// 		{
		if (Health > 0){
			Health = FMath::Min(float(PawnReplicationInfo.HealthMax), Health + (PawnReplicationInfo.HealthRegenerationAmount * DeltaTime));
			Mana = FMath::Min(float(PawnReplicationInfo.ManaMax), Mana + (PawnReplicationInfo.ManaRegenerationAmount * DeltaTime));
		}
			
/*		}*/
	}

}

void ALabPawn::BeginPlay()
{
	Super::BeginPlay();

	//UE_LOG(LogLab, Log, TEXT("ALabPawn::BeginPlay()"));
	//PawnReplicationInfo = ConstructObject<ULabPawnReplicationInfo>(ULabPawnReplicationInfo::StaticClass(), this);
	
		
	if (HasAuthority())
	{
		StatsModifier = ConstructObject<ULabStatsModifier>(ULabStatsModifier::StaticClass(), this);
		if (StatsModifier)
		{
			//StatsModifier->AddStatChange(STATNAME_HPMax, 10, MODTYPE_Growth, 0.f, true);
			//StatsModifier.AddStatChange(STATNAME_Agility, BaseAgility, MODTYPE_Addition, 0.f, true);
			//StatsModifier.AddStatChange(STATNAME_Intelligence, BaseIntelligence, MODTYPE_Addition, 0.f, true);
		}
	}
		
}


void ALabPawn::RecalculateStats()
{
	if (!HasAuthority())
		return;

	if (StatsModifier == NULL)
	{
		return;
	}

// 	if (PawnReplicationInfo != NULL)
// 	{
		// MUST calc attributes first
		//UDKMOBAPawnReplicationInfo.Strength = StatsModifier.CalculateStat(STATNAME_Strength);
		//UDKMOBAPawnReplicationInfo.Agility = StatsModifier.CalculateStat(STATNAME_Agility);
		//UDKMOBAPawnReplicationInfo.Intelligence = StatsModifier.CalculateStat(STATNAME_Intelligence);

		// Now can go through the rest
	PawnReplicationInfo.ManaMax = StatsModifier->CalculateStat(STATNAME_ManaMax, BaseMana);
	PawnReplicationInfo.HealthMax = StatsModifier->CalculateStat(STATNAME_HPMax, BaseHealth);
	PawnReplicationInfo.Damage = StatsModifier->CalculateStat(STATNAME_Damage, BaseDamage);
		// If just spawned, then set Mana  and Health to Max

		if (JustSpawned)
		{
			JustSpawned = false;
			Health = PawnReplicationInfo.HealthMax;
		}
/*	}*/
}

float ALabPawn::GetDamage() const
{
	return PawnReplicationInfo.Damage;
}

float ALabPawn::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//ULabBlueprintLibrary::printDebugInfo("Got damage=" + FString::SanitizeFloat(DamageAmount) + "from" + DamageCauser->GetName());

	if (Health <= 0.f)
	{
		// no further damage if already dead
		return 0.f;
	}

	// Modify based on game rules.
	ALabGameMode* const Game = GetWorld()->GetAuthGameMode<ALabGameMode>();
	DamageAmount = Game ? Game->ModifyDamage(DamageAmount, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;
	DamageAmount = AdjustDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}
		else
		{
			PlayHit(ActualDamage, DamageEvent, EventInstigator ? EventInstigator->GetPawn() : NULL, DamageCauser);
		}
		// broadcast AI-detectable noise
		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);

		// If our gamestate wants to know when damage happens
		/*AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (GameState)
		{
			GameState->OnActorDamaged(this, ActualDamage, EventInstigator);
		}*/
	}

	return ActualDamage;
}


float ALabPawn::ServerPlayAttackMontage()
{
	float duration = 0.f;
	if (HasAuthority())
	{
		if (MeleeAnim)
		{
			duration = PlayAnimMontage(MeleeAnim);
			BroadcastPlayAttackMontage();
		}

	}

	return duration;
}

void ALabPawn::BroadcastPlayAttackMontage_Implementation()
{
	PlayAnimMontage(MeleeAnim);
}


bool ALabPawn::CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| Role != ROLE_Authority						// not authority
		|| GetWorld()->GetAuthGameMode() == NULL
		|| GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return false;
	}

	return true;
}


bool ALabPawn::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser))
	{
		return false;
	}

	Health = FMath::Max(0.0f, (float)Health);

	// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	//AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	//GetWorld()->GetAuthGameMode<AShooterGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	NetUpdateFrequency = GetDefault<ALabPawn>()->NetUpdateFrequency;
	CharacterMovement->ForceReplicationUpdate();

	OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	return true;
}

void ALabPawn::OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (bIsDying)
	{
		return;
	}

	bReplicateMovement = false;
	bTearOff = true;
	bIsDying = true;

	if (Role == ROLE_Authority)
	{
		ReplicateHit(KillingDamage, DamageEvent, PawnInstigator, DamageCauser, true);
	}

	// cannot use IsLocallyControlled here, because even local client's controller may be NULL here
	/*if (GetNetMode() != NM_DedicatedServer && DeathSound && Mesh1P && Mesh1P->IsVisible())
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}*/


	DetachFromControllerPendingDestroy();
	StopAllAnimMontages();

	// disable collisions on capsule
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	if (Mesh)
	{
		static FName CollisionProfileName(TEXT("Ragdoll"));
		Mesh->SetCollisionProfileName(CollisionProfileName);
	}
	SetActorEnableCollision(true);

	// Death anim
	float DeathAnimDuration = PlayAnimMontage(DeathAnim);

	// Ragdoll
	if (DeathAnimDuration > 0.f)
	{
		GetWorldTimerManager().SetTimer(this, &ALabPawn::SetRagdollPhysics, FMath::Min(0.1f, DeathAnimDuration), false);
	}
	else
	{
		SetRagdollPhysics();
	}
}

void ALabPawn::PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser)
{
	if (Role == ROLE_Authority)
	{
		ReplicateHit(DamageTaken, DamageEvent, PawnInstigator, DamageCauser, false);
	}
}


void ALabPawn::SetRagdollPhysics()
{

	bool bInRagdoll = false;

	if (IsPendingKill())
	{
		bInRagdoll = false;
	}
	else if (!Mesh || !Mesh->GetPhysicsAsset())
	{
		bInRagdoll = false;
	}
	else
	{
		// initialize physics/etc
		Mesh->SetAllBodiesSimulatePhysics(true);
		Mesh->SetSimulatePhysics(true);
		Mesh->WakeAllRigidBodies();
		Mesh->bBlendPhysics = true;

		bInRagdoll = true;
	}

	CharacterMovement->StopMovementImmediately();
	CharacterMovement->DisableMovement();
	CharacterMovement->SetComponentTickEnabled(false);

	if (!bInRagdoll)
	{
		// hide and set short lifespan
		TurnOff();
		SetActorHiddenInGame(true);
		SetLifeSpan(1.0f);
	}
	else
	{
		SetLifeSpan(5.0f);
	}
}



void ALabPawn::ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser, bool bKilled)
{
	const float TimeoutTime = GetWorld()->GetTimeSeconds() + 0.5f;

	FDamageEvent const& LastDamageEvent = LastTakeHitInfo.GetDamageEvent();
	if ((PawnInstigator == LastTakeHitInfo.PawnInstigator.Get()) && (LastDamageEvent.DamageTypeClass == LastTakeHitInfo.DamageTypeClass) && (LastTakeHitTimeTimeout == TimeoutTime))
	{
		// same frame damage
		if (bKilled && LastTakeHitInfo.bKilled)
		{
			// Redundant death take hit, just ignore it
			return;
		}

		// otherwise, accumulate damage done this frame
		Damage += LastTakeHitInfo.ActualDamage;
	}

	LastTakeHitInfo.ActualDamage = Damage;
	LastTakeHitInfo.PawnInstigator = Cast<ALabPawn>(PawnInstigator);
	LastTakeHitInfo.DamageCauser = DamageCauser;
	LastTakeHitInfo.SetDamageEvent(DamageEvent);
	LastTakeHitInfo.bKilled = bKilled;
	LastTakeHitInfo.EnsureReplication();

	LastTakeHitTimeTimeout = TimeoutTime;
}

void ALabPawn::OnRep_LastTakeHitInfo()
{
	if (LastTakeHitInfo.bKilled)
	{
		ULabBlueprintLibrary::printDebugInfo("ALabPawn::OnRep_LastTakeHitInfo: killed");
		OnDeath(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
	else
	{
		ULabBlueprintLibrary::printDebugInfo("ALabPawn::OnRep_LastTakeHitInfo: been hit");
		PlayHit(LastTakeHitInfo.ActualDamage, LastTakeHitInfo.GetDamageEvent(), LastTakeHitInfo.PawnInstigator.Get(), LastTakeHitInfo.DamageCauser.Get());
	}
}

void ALabPawn::OnRep_Test()
{
	if (HasAuthority())
		ULabBlueprintLibrary::printDebugInfo("Server" + GetName() + "OnRep_Test");
	else
		ULabBlueprintLibrary::printDebugInfo("Client" + GetName() + "OnRep_Test");
}

void ALabPawn::StopAllAnimMontages()
{
	
	if (Mesh && Mesh->AnimScriptInstance)
	{
		Mesh->AnimScriptInstance->Montage_Stop(0.0f);
	}
}

float ALabPawn::AdjustDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	/*
	local float OutDamage;
	local int i;
	local UDKMOBAPawnReplicationInfo UDKMOBAPawnReplicationInfo;

	OutDamage = InDamage;
	UDKMOBAPawnReplicationInfo = UDKMOBAPawnReplicationInfo(PlayerReplicationInfo);
	if (UDKMOBAPawnReplicationInfo != None)
	{
		// First, check for evasion
		if (DamageType == class'UDKMOBADamageTypePhysical')
		{
			if (FRand() < UDKMOBAPawnReplicationInfo.Evasion)
			{
				// missed!
				InDamage = 0;
				return;
			}
		}
		// And magic evasion...
		else if (DamageType == class'UDKMOBADamageTypeMagical')
		{
			if (UDKMOBAPawnReplicationInfo.MagicImmunity || FRand() < UDKMOBAPawnReplicationInfo.MagicEvasion)
			{
				// immune/missed!
				InDamage = 0;
				return;
			}
			else
			{
				OutDamage *= UDKMOBAPawnReplicationInfo.MagicResistance;
			}
		}

		// Iterate through spells, and allow them to adjust damage
		for (i = 0; i < UDKMOBAPawnReplicationInfo.Spells.Length; ++i)
		{
			if (UDKMOBAPawnReplicationInfo.Spells[i] != None)
			{
				UDKMOBAPawnReplicationInfo.Spells[i].AdjustDamage(OutDamage, Momentum, InstigatedBy, HitLocation, DamageType, HitInfo, DamageCauser);
			}
		}

		// Iterate through items, and allow them to adjust damage
		for (i = 0; i < UDKMOBAPawnReplicationInfo.Items.Length; ++i)
		{
			if (UDKMOBAPawnReplicationInfo.Items[i] != None)
			{
				UDKMOBAPawnReplicationInfo.Items[i].AdjustDamage(OutDamage, Momentum, InstigatedBy, HitLocation, DamageType, HitInfo, DamageCauser);
			}
		}

		// Now handle armor - these formulas taken from DotA
		if (UDKMOBAPawnReplicationInfo.Armor > 0.f)
		{
			OutDamage *= 1.f - ((0.06f * UDKMOBAPawnReplicationInfo.Armor) / (1.06f * UDKMOBAPawnReplicationInfo.Armor));
		}
		else if (UDKMOBAPawnReplicationInfo.Armor < 0.f)
		{
			OutDamage *= 2.f - (0.94f ** Abs(UDKMOBAPawnReplicationInfo.Armor));
		}
	}

	InDamage = Int(OutDamage);*/
	return DamageAmout;
}
