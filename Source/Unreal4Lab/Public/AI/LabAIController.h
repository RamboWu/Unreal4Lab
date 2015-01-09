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

	/** Component used to detect*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	TSubobjectPtr<class USphereComponent> SensingComponent;

	virtual void Possess(class APawn *InPawn) override;

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void SearchForEnemy();

	UFUNCTION(BlueprintCallable, Category = Behavior)
	bool withInAttackRange();

	/*Server */
	UFUNCTION(BlueprintCallable, Category = Behavior)
	void Attack();

	// Begin AIController Interface
	virtual void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result) override;
	// End AIController Interface

	UFUNCTION()
	void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

	class ALabAIRoute* Route;

protected:

	void SetEnemy(class APawn *InPawn);
	void SetDestination(int32 index);

	/** move closer to target */
	void MoveCloser();

	/** Checks actor and returns true if valid */
	bool IsTargetValid(AActor* InActor) const;

	uint8 EnemyKeyID;
	uint8 EnemyLocationID;

	/** time when we will finish playing melee animation */
	float MeleeAttackAnimationEndTime;

	/** if pawn is playing attack animation */
	uint32 bIsPlayingAnimation : 1;

	/** set to true when we are moving to our target */
	uint32 bMovingToTarget : 1;

	TArray<class AActor*>	AllTargets;

	/** Current selected target to attack */
	class AActor*			CurrentTarget;

private:
	int32  cur_route_node_index;
};
