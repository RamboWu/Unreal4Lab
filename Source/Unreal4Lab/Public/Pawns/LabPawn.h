// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LabTypes.h"
#include "LabPawnReplicationInfo.h"
#include "LabTeamInterface.h"
#include "LabStateInterface.h"
#include "LabAttackInterface.h"
#include "GameFramework/Character.h"
#include "LabPawn.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabPawn : public ACharacter, 
	public ILabTeamInterface, public ILabStateInterface, public ILabAttackInterface
{
	GENERATED_UCLASS_BODY()

	/** Identifies if pawn is in its dying state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Health)
	uint32 bIsDying : 1;

	/*ILabStateInterface begin*/
	virtual uint8 GetTeamNum() const override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual int32 GetHealth() const override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual float GetHealthPercentage() const override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual int32 GetMaxHealth() const override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual int32 GetAttackRange() const override;

	virtual int32 GetSightDistance() const override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual float GetDamage() const override;
	/*ILabStateInterface end*/

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum);

	//UFUNCTION(NetMulticast, Unreliable)
	//void Client_PlayMeleeAnim();

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Test, Category = State)
	struct FLabPawnReplicationInfo  PawnReplicationInfo;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;
	//减去防御力，减去装备，技能的低档，圣神伤害等等
	virtual float AdjustDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	/**
	* Kills pawn.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	//UFUNCTION(NetMulticast, Unreliable)
	//virtual void BroadcastDeathAnimMontage();

	virtual float ServerPlayAttackMontage();
	
	UFUNCTION(NetMulticast, Unreliable)
	virtual void BroadcastPlayAttackMontage();

	

protected:
	/** Returns True if the pawn can die in the current state */
	virtual bool CanDie(float KillingDamage, FDamageEvent const& DamageEvent, AController* Killer, AActor* DamageCauser) const;

	/** notification when killed, for both the server and client. */
	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);

	/** play effects on hit */
	virtual void PlayHit(float DamageTaken, struct FDamageEvent const& DamageEvent, class APawn* PawnInstigator, class AActor* DamageCauser);

	/** switch to ragdoll */
	void SetRagdollPhysics();

	/** sets up the replication for taking a hit */
	void ReplicateHit(float Damage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser, bool bKilled);

	/** play hit or death on client */
	UFUNCTION()
	void OnRep_LastTakeHitInfo();

	/** play hit or death on client */
	UFUNCTION()
	void OnRep_Test();

	/** Replicate where this pawn was last hit and damaged */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_LastTakeHitInfo)
	struct FTakeHitInfo LastTakeHitInfo;

	/** Time at which point the last take hit info for the actor times out and won't be replicated; Used to stop join-in-progress effects all over the screen */
	float LastTakeHitTimeTimeout;

	/** stop playing all montages */
	void StopAllAnimMontages();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = State)
	int32 Mana;

protected:

	/** melee anim */
	UPROPERTY(EditDefaultsOnly, Category = View)
	class UAnimMontage* MeleeAnim;

	/** death anim */
	UPROPERTY(EditDefaultsOnly, Category = View)
	class UAnimMontage* DeathAnim;

	/*team num*/
	UPROPERTY(EditAnywhere, Replicated,  Category = State)
	TEnumAsByte<ELabTeam::Type> m_team_num;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = State)
	int32 Health;

	/*base health*/
	UPROPERTY(EditAnywhere, Category = State)
	int32 BaseHealth;

	UPROPERTY(EditAnywhere, Category = State)
	int32 BaseMana;

	/*base attack range*/
	UPROPERTY(EditAnywhere, Category = State)
	int32 BaseAttackDistance;

	UPROPERTY(EditAnywhere, Category = State)
	int32 BaseSightDistance;

	UPROPERTY(EditAnywhere, Category = State)
	int32 BaseDamage;

	UPROPERTY()
	class ULabStatsModifier* StatsModifier;

	int32 JustSpawned:1;

	/**
	* Recalculate the pawn stat
	*
	* @network		Server
	*/
	void RecalculateStats();

	/** event called after die animation  to hide character and delete it asap */
	//void OnDieAnimationEnd();
};
