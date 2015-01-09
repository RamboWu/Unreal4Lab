#pragma once

#include "LabStateInterface.generated.h"

/** Interface for actors which can be associated with state */
UINTERFACE()
class ULabStateInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()


};

class ILabStateInterface
{
	GENERATED_IINTERFACE_BODY()

	virtual int32 GetHealth() const;

	virtual float GetHealthPercentage() const;

	virtual int32 GetMaxHealth() const;

	virtual bool HasMana() const;

	virtual int32 getMana() const;

	virtual float GetManaPercentage() const;

	virtual int32 GetAttackRange() const;

	virtual int32 GetSightDistance() const;
};
