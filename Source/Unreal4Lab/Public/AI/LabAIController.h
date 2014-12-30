// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "LabAIController.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabAIController : public AAIController
{
	GENERATED_UCLASS_BODY()


	UPROPERTY(Transient)
	TSubobjectPtr<class UBlackboardComponent> BlackboardComp;

	UPROPERTY(Transient)
	TSubobjectPtr<class UBehaviorTreeComponent> BehaviorComp;

	virtual void Possess(class APawn *InPawn) override;

	void SetEnemy(class APawn *InPawn);

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void SearchForEnemy();

	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool withInAttackRange();

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void Attack();

protected:
	uint8 EnemyKeyID;
	uint8 EnemyLocationID;

	/** time when we will finish playing melee animation */
	float MeleeAttackAnimationEndTime;

	/** if pawn is playing attack animation */
	uint32 bIsPlayingAnimation : 1;
};
