#include "Unreal4Lab.h"
#include "LabStateInterface.h"

ULabStateInterface::ULabStateInterface(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

int32 ILabStateInterface::GetHealth() const
{
	return 0;
}

float ILabStateInterface::GetHealthPercentage() const
{
	return 0.f;
}

bool ILabStateInterface::HasMana() const
{
	return false;
}

int32 ILabStateInterface::getMana() const
{
	return 0;
}

float ILabStateInterface::GetManaPercentage() const
{
	return 0.f;
}

int32 ILabStateInterface::GetMaxHealth() const
{
	return 0;
}

int32 ILabStateInterface::GetAttackRange() const
{
	return 0;
}

int32 ILabStateInterface::GetSightDistance() const
{
	return 0;
}