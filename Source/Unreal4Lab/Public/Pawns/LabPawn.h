// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LabTypes.h"
#include "LabTeamInterface.h"
#include "LabStateInterface.h"
#include "GameFramework/Character.h"
#include "LabPawn.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabPawn : public ACharacter, public ILabTeamInterface, public ILabStateInterface
{
	GENERATED_UCLASS_BODY()

	
	/*ILabStateInterface begin*/
	virtual uint8 GetTeamNum() const override;

	virtual uint32 GetHealth() override;

	virtual float GetHealthPercentage() override;

	virtual uint32 GetMaxHealth() override;

	UFUNCTION(BlueprintCallable, Category = State)
	virtual uint32 GetAttackRange() const override;

	virtual uint32 GetSightDistance() override;
	/*ILabStateInterface end*/

	/** set team number */
	void SetTeamNum(uint8 NewTeamNum);


	UFUNCTION(NetMulticast, Unreliable)
	void Client_PlayMeleeAnim();


	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	class ULabPawnReplicationInfo* PawnReplicationInfo;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	/** melee anim */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* MeleeAnim;

	/** death anim */
	UPROPERTY(EditDefaultsOnly, Category = Pawn)
	UAnimMontage* DeathAnim;

	/*team num*/
	UPROPERTY(EditAnywhere, Replicated,  Category = State)
	TEnumAsByte<ELabTeam::Type> m_team_num;

	/*base health*/
	UPROPERTY(EditAnywhere, Category = State)
	uint32 BaseHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = State)
	uint32 Health;

	/*base attack range*/
	UPROPERTY(EditAnywhere, Category = State)
	uint32 m_base_attack_range;

	UPROPERTY(EditAnywhere, Category = State)
	uint32 m_base_sight_distance;

	class ULabStatsModifier* StatsModifier;

	uint32 JustSpawned:1;

	/**
	* Recalculate the pawn stat
	*
	* @network		Server
	*/
	void RecalculateStats();
};
