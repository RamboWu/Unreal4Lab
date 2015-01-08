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
	Health(0),
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
	DOREPLIFETIME(ALabPawn, PawnReplicationInfo);
}

uint8 ALabPawn::GetTeamNum() const
{
	return m_team_num;
}

void ALabPawn::SetTeamNum(uint8 NewTeamNum)
{
	m_team_num = (TEnumAsByte<ELabTeam::Type>)NewTeamNum;
}

uint32 ALabPawn::GetHealth()
{
	return Health;
}

float ALabPawn::GetHealthPercentage()
{
	return ((float)Health / GetMaxHealth());
}

uint32 ALabPawn::GetMaxHealth()
{
	return BaseHealth;
}

uint32 ALabPawn::GetAttackRange() const
{
	return BaseAttackDistance;
}

void ALabPawn::Client_PlayMeleeAnim_Implementation()
{

	if ((Health > 0.f) && MeleeAnim)
	{
		float duration = PlayAnimMontage(MeleeAnim);
	}
}

void ALabPawn::Client_PlayDeathAnim_Implementation()
{
	if (DeathAnim)
	{
		float duration = PlayAnimMontage(DeathAnim);
	}
}

uint32 ALabPawn::GetSightDistance()
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
		if (PawnReplicationInfo != NULL)
		{
			Health = FMath::Min(float(PawnReplicationInfo->HealthMax), Health + (PawnReplicationInfo->HealthRegenerationAmount * DeltaTime));
			//Mana = FMin(UDKMOBAPawnReplicationInfo.ManaMax, Mana + (UDKMOBAPawnReplicationInfo.ManaRegenerationAmount * DeltaTime));
		}
	}

}

void ALabPawn::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		UE_LOG(LogLab, Log, TEXT("ALabPawn::BeginPlay()"));
		PawnReplicationInfo = ConstructObject<ULabPawnReplicationInfo>(ULabPawnReplicationInfo::StaticClass(), this);
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

	if (PawnReplicationInfo != NULL)
	{
		// MUST calc attributes first
		//UDKMOBAPawnReplicationInfo.Strength = StatsModifier.CalculateStat(STATNAME_Strength);
		//UDKMOBAPawnReplicationInfo.Agility = StatsModifier.CalculateStat(STATNAME_Agility);
		//UDKMOBAPawnReplicationInfo.Intelligence = StatsModifier.CalculateStat(STATNAME_Intelligence);

		// Now can go through the rest
		//UDKMOBAPawnReplicationInfo.ManaMax = StatsModifier.CalculateStat(STATNAME_ManaMax, BaseMana);

		PawnReplicationInfo->HealthMax = StatsModifier->CalculateStat(STATNAME_HPMax, BaseHealth);
		PawnReplicationInfo->Damage = StatsModifier->CalculateStat(STATNAME_Damage, BaseDamage);
		// If just spawned, then set Mana  and Health to Max

		if (JustSpawned)
		{
			JustSpawned = false;
			Health = PawnReplicationInfo->HealthMax;
		}
	}
}

float ALabPawn::GetDamage() const
{
	return PawnReplicationInfo->Damage;
}

float ALabPawn::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	//Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ULabBlueprintLibrary::printDebugInfo("Got damage=" + FString::SanitizeFloat(DamageAmount) + "from" + DamageCauser->GetName());

	if (Health <= 0.f)
	{
		// no further damage if already dead
		return 0.f;
	}

	// Modify based on game rules.
	ALabGameMode* const Game = GetWorld()->GetAuthGameMode<ALabGameMode>();
	DamageAmount = Game ? Game->ModifyDamage(DamageAmount, this, DamageEvent, EventInstigator, DamageCauser) : 0.f;

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		if (Health <= 0.f)
		{
			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
		}

		// broadcast AI-detectable noise
		MakeNoise(1.0f, EventInstigator ? EventInstigator->GetPawn() : this);

		// our gamestate wants to know when damage happens
		/*AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (GameState)
		{
			GameState->OnActorDamaged(this, ActualDamage, EventInstigator);
		}*/
	}

	return ActualDamage;
}

void ALabPawn::Die(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser)
{
	if (bIsDying										// already dying
		|| IsPendingKill()								// already destroyed
		|| GetWorld()->GetAuthGameMode() == NULL
		|| GetWorld()->GetAuthGameMode()->GetMatchState() == MatchState::LeavingMap)	// level transition occurring
	{
		return;
	}

	bIsDying = true;
	//Health = FMath::Min(0.0f, Health);

	// figure out who killed us
	UDamageType const* const DamageType = DamageEvent.DamageTypeClass ?
		Cast<const UDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject())
		: GetDefault<UDamageType>();
	Killer = GetDamageInstigator(Killer, *DamageType);

	// forcibly end any timers that may be in flight
	GetWorldTimerManager().ClearAllTimersForObject(this);

	// notify the game mode if an Enemy dies
	/*if (GetTeamNum() == EStrategyTeam::Enemy)
	{
		AStrategyGameState* const GameState = GetWorld()->GetGameState<AStrategyGameState>();
		if (GameState)
		{
			GameState->OnCharDied(this);
		}
	}*/

	// disable any AI
	/*AStrategyAIController* const AIController = Cast<AStrategyAIController>(Controller);
	if (AIController)
	{
		AIController->EnableLogic(false);
	}*/

	// turn off collision
	if (CapsuleComponent.IsValid())
	{
		CapsuleComponent->BodyInstance.SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CapsuleComponent->BodyInstance.SetResponseToChannel(ECC_Pawn, ECR_Ignore);
		CapsuleComponent->BodyInstance.SetResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	}

	// turn off movement
	if (CharacterMovement)
	{
		CharacterMovement->StopMovementImmediately();
		CharacterMovement->DisableMovement();
	}

	// detach the controller
	if (Controller != NULL)
	{
		Controller->UnPossess();
	}

	// play death animation
	float DeathAnimDuration = 2.f;
	if (DeathAnim)
	{
		Client_PlayDeathAnim();
		//DeathAnimDuration = PlayAnimMontage(DeathAnim) / (Mesh.IsValid() && Mesh->GlobalAnimRateScale > 0 ? Mesh->GlobalAnimRateScale : 1);
		//UAnimInstance * AnimInstance = (Mesh) ? Mesh->GetAnimInstance() : NULL;
	}
	ULabBlueprintLibrary::printDebugInfo("Death Animation Duration=" + FString::SanitizeFloat(DeathAnimDuration));
	GetWorldTimerManager().SetTimer(this, &ALabPawn::OnDieAnimationEnd, DeathAnimDuration + 0.01, false);
}

void ALabPawn::OnDieAnimationEnd()
{
	this->SetActorHiddenInGame(true);
	// delete the pawn asap
	SetLifeSpan(0.01f);
}