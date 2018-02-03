#include "Pneumatics.h"

Pneumatics::Pneumatics(uint32_t InSoleChannel, uint32_t OutSoleChannel):
	InSole(InSoleChannel),
	OutSole(OutSoleChannel)
{    }

void Pneumatics::set(bool state)
{
	if (InSole.Get())
	{
		InSole.Set(!state);
		OutSole.Set(state);
	}
	else
	{
		OutSole.Set(state);
		InSole.Set(!state);
	}
}

bool Pneumatics::get()
{
		return OutSole.Get();
}
