// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LabAIRoute.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabAIRoute : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Route)
	TSubobjectPtr<class UStaticMeshComponent> Mark;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Route)
	TArray<class AActor*> all_targets;

};
