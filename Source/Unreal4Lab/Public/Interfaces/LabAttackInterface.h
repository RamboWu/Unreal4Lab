#pragma once

#include "LabAttackInterface.generated.h"

/** Interface for actors which can be associated with state */
UINTERFACE()
class ULabAttackInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()


};

class ILabAttackInterface
{
	GENERATED_IINTERFACE_BODY()
	/**
	* Returns the attack priority of this actor
	*
	* @param		Attacker		Actor that wants to attack this UDKMOBAAttackInterface
	* @return						Returns the attacking priority that this implementing actor belongs to
	*/
	virtual uint32 GetAttackPriority(AActor Attacker) const;
	/**
	* Returns the enemy
	*
	* @return		Returns the current enemy
	*/
	virtual class AActor* GetEnemy() const;
	/**
	* Returns true if the actor is still valid to attack
	*
	* @return		Returns true if the actor is still valid to attack
	*/
	virtual bool IsValidToAttack() const;
	/**
	* Returns the amount of damage that this actor does for an attack. NOT used for spells.
	*
	* @return		The amount of damage done, with Blind taken account of.
	*/
	virtual float GetDamage() const;
	/**
	* Returns the amount of damage that this actor does for an attack.
	*
	* @return		The type of damage this actor does for auto-attacks.
	*/
	//virtual class<DamageType> GetDamageType();
};
