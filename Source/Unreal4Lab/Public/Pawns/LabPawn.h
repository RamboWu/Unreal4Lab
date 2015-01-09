// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LabTypes.h"
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

	virtual int32 GetHealth() const override;

	virtual float GetHealthPercentage() const override;

	virtual int32 GetMaxHealth() const override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual int32 GetAttackRange() const override;

	virtual int32 GetSightDistance() const override;

	virtual float GetDamage() const override;
	/*ILabStateInterface end*/

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum);

	//UFUNCTION(NetMulticast, Unreliable)
	//void Client_PlayMeleeAnim();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	class ULabPawnReplicationInfo* PawnReplicationInfo;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	/**
	* Kills pawn.
	* @param KillingDamage - Damage amount of the killing blow
	* @param DamageEvent - Damage event of the killing blow
	* @param Killer - Who killed this pawn
	* @param DamageCauser - the Actor that directly caused the damage (i.e. the Projectile that exploded, the Weapon that fired, etc)
	* @returns true if allowed
	*/
	virtual void Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);

	UFUNCTION(NetMulticast, Unreliable)
	virtual void BroadcastDeathAnimMontage();

	virtual float ServerPlayAttackMontage();
	
	UFUNCTION(NetMulticast, Unreliable)
	virtual void BroadcastPlayAttackMontage();

	/** melee anim */
	UPROPERTY(EditDefaultsOnly, Category = View)
	class UAnimMontage* MeleeAnim;

	/** death anim */
	UPROPERTY(EditDefaultsOnly, Category = View)
	class UAnimMontage* DeathAnim;

protected:
	/*team num*/
	UPROPERTY(EditAnywhere, Replicated,  Category = State)
	TEnumAsByte<ELabTeam::Type> m_team_num;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = State)
	int32 Health;

	/*base health*/
	UPROPERTY(EditAnywhere, Category = State)
	int32 BaseHealth;

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
	void OnDieAnimationEnd();
};
