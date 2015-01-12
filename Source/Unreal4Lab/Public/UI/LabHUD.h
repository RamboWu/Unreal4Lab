// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "LabHUD.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL4LAB_API ALabHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

public:

	// Begin HUD interface
	virtual void DrawHUD() override;
	// End HUD interface

	/** current UI scale */
	float UIScale;

	/** gets player controller */
	class ALabPlayerController* GetPlayerController() const;
	class ALabPlayerState* GetPlayerState() const;

protected:

	/**
	* Draws health bar for specific actor.
	*
	* @param	ForActor	Actor for which the health bar is for.
	* @param	HealthPct	Current Health percentage.
	* @param	BarHeight	Height of the health bar
	* @param	OffsetY		Y Offset of the health bar.
	*/
	void DrawHealthBar(AActor* ForActor, float HealthPct, int32 BarHeight, int OffsetY = 0) const;

	/** draw health bars for actors */
	void DrawActorsHealth();

	/** gray health bar texture */
	UPROPERTY()
	class UTexture2D* BarFillTexture;

	/** player team health bar texture */
	UPROPERTY()
	class UTexture2D* PlayerTeamHPTexture;

	/** enemy team health bar texture */
	UPROPERTY()
	class UTexture2D* EnemyTeamHPTexture;
};
