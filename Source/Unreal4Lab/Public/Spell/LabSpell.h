// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LabSpell.generated.h"

/**
 * 
 */
// The different ways a spell can be aimed
UENUM(BlueprintType)
enum EAimType
{
	EAT_World		UMETA(DisplayName = "World"),  // we trace against the level only
	EAT_Actor		UMETA(DisplayName = "Actor"), // will trace against any actor
	EAT_None		UMETA(DisplayName = "No Aiming") // no aim, just fire
};

UCLASS()
class UNREAL4LAB_API ALabSpell : public AActor
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	class UTexture2D* SpellIcon;
	
	// The maximum level that the spell can be upgraded to
	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	int32 MaxLevel;
	// Mana cost to use the active ability of this spell - by level
	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	TArray<float> ManaCost;
	// Whether this spell has an active ability at all.
	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	bool HasActive;
	// Whether this spell needs the user to choose where to fire it
	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	TEnumAsByte<EAimType> AimingType;

	// Decal material to use, only on PC
	/*var(Spell, Aim) const MaterialInterface DecalMaterial;
	// Decal material size
	var(Spell, Aim) const Vector2D DecalSize;

	// Sound to play when activating the spell
	var(Spell, Active) const SoundCue ActivatingSoundCue;
	// Particle effect to create when activating the spell
	var(Spell, Active) const ParticleSystem ActivatingParticleTemplate;
	// Sound to play when casting the spell
	var(Spell, Active) const SoundCue CastingSoundCue;
	// Particle effect to create when casting the spell
	var(Spell, Active) const ParticleSystem CastingParticleTemplate;*/
	// Cooldown time. Time it takes for this spell to refresh after it has been cast - by level
	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	TArray<float> CooldownTime;
	// How long (in seconds) from triggering the ability before damage/etc is actually dealt
	UPROPERTY(EditDefaultsOnly, Category = "Spell")
	TArray<float> ActivationTime;
	// What sort of attack type a projectile or damage given uses
	//var(Spell, Active) const UDKMOBAPawn.EAttackType AttackType;

	// Used if AimingType == EAT_World to say where the spell is going
	FVector TargetLocation;
	// Used if AimingType == EAT_Actor to say where the spell is going
	class AActor* TargetActor;
	// Index within the Spells array. This is because the ordering of the Spells arrays are important!
	UPROPERTY(EditAnywhere, Replicated, Category = "Spell")
	int32 OrderIndex;
	// Current upgrade level of the spell
	UPROPERTY(EditAnywhere, Replicated, Category = "Spell")
	int32 Level;
	// Player replication info that owns this spell.
	//var RepNotify UDKMOBAPawnReplicationInfo OwnerReplicationInfo;
	// Whether this spell is channeling right now
	//var ProtectedWrite RepNotify bool IsChanneling;
	// When this spell was (last) triggered
	//var ProtectedWrite float ActivatedAt;
	// Pawn that currently owns this spell
	class APawn* PawnOwner;
	// Activation count which increases when the spell is activated. If the activation count gets reset back to zero, then it is considered to be refreshed
	//var ProtectedWrite RepNotify int ActivationCount;
	// Cast count which increases when the spell is cast. If the cast count gets reset back to zero, then it is considered to be refreshed
	//var ProtectedWrite RepNotify int CastCount;

	/**
	* Returns true if the spell has an active ability that can be cast
	*/
	
	bool CanCast();
	//both server and client
	void Activate();
	//TODO how to set target
	bool SetTargetsFromAim(FVector AimLocation, FVector AimDirection);
	virtual float AdjustDamage(float DamageAmout, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);


protected:
	void Cast();
	virtual void PerformCast();
	virtual void CooldownTimer();

	void SendPlayerMessage(FText Msg);
	float GetManaCost();
	float GetActivationTime();
	float GetCooldownTime();
	void ActivateEffects(bool ViaReplication);
	void CastEffects(bool ViaReplication);

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Spell")
	bool bInCoolDown;
};
