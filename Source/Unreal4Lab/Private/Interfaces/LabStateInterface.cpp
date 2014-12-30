#include "Unreal4Lab.h"
#include "LabStateInterface.h"

ULabStateInterface::ULabStateInterface(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{

}

uint32 ILabStateInterface::GetHealth()
{
	return 0;
}

float ILabStateInterface::GetHealthPercentage()
{
	return 0.f;
}

bool ILabStateInterface::HasMana()
{
	return false;
}

uint32 ILabStateInterface::getMana()
{
	return 0;
}

float ILabStateInterface::GetManaPercentage()
{
	return 0.f;
}
