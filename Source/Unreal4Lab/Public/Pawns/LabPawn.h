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
	/*ILabStateInterface end*/


	/** set team number */
	void SetTeamNum(uint8 NewTeamNum);



protected:
	/*team num*/
	UPROPERTY(EditAnywhere, Replicated,  Category = State)
	TEnumAsByte<ELabTeam::Type> m_team_num;

	/*base health*/
	UPROPERTY(EditAnywhere, Category = State)
	uint32 m_base_health;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = State)
	uint32 m_health;
};
